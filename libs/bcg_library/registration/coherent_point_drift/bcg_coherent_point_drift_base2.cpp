//
// Created by alex on 10.02.21.
//

#include "bcg_coherent_point_drift_base2.h"
#include "utils/bcg_timer.h"
#include "bcg_property_map_eigen.h"
#include "tbb/tbb.h"
#include "math/rotations/bcg_rotation_project_on_so.h"
#include "math/matrix/bcg_matrix_pairwise_distances.h"

namespace bcg {


void coherent_point_drift_base::init(vertex_container *source_vertices, Transform &source_model,
                                     vertex_container *target_vertices, Transform &target_model) {
    this->source_model = &source_model;
    this->target_model = &target_model;
    backup_source_model = source_model;
    backup_target_model = target_model;
    P1 = source_vertices->get_or_add<bcg_scalar_t, 1>("v_cpd_P1");
    PX = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_PX");
    PT1 = target_vertices->get_or_add<bcg_scalar_t, 1>("v_cpd_PT1");
    source_positions = source_vertices->get<VectorS<3>, 3>("v_position");
    target_positions = target_vertices->get<VectorS<3>, 3>("v_position");
    target_kdtree.build(target_positions);
    M = source_positions.size();
    N = target_positions.size();
    D = source_positions[0].size();
    X = transform_target();
    Y = transform_source();
    sigma_squared = pairwise_distance_squared(X, Y).sum() / kernel_precision(M * N * D);
}

void coherent_point_drift_base::reset() {
    if (source_model != nullptr) {
        *source_model = backup_source_model;
        X = transform_target();
    }
    if (target_model != nullptr) {
        *target_model = backup_target_model;
        Y = transform_source();
    }
}

void coherent_point_drift_base::update_P() {
    Timer timer;
    if (std::sqrt(sigma_squared) < kdtree_sigma_threshold) {
        softmatching_type = SoftmatchingType::kdtree;
    }
    Y = transform_source();
    switch (softmatching_type) {
        case SoftmatchingType::full : {
            update_P_full();
            std::cout << "P_full: ";
            break;
        }
        case SoftmatchingType::full_FGT : {
            update_P_FGT();
            std::cout << "P_full_FGT: ";
            break;
        }
        case SoftmatchingType::parallel : {
            update_P_parallel(parallel_grain_size);
            std::cout << "P_parallel: ";
            break;
        }
        case SoftmatchingType::nystroem : {
            update_P_nystroem();
            std::cout << "P_nystroem: ";
            break;
        }
        case SoftmatchingType::nystroem_FGT : {
            update_P_nystroem_FGT();
            std::cout << "P_nystroem_FGT: ";
            break;
        }
        case SoftmatchingType::kdtree : {
            update_P_kdtree(parallel_grain_size);
            std::cout << "P_kdtree: ";
            break;
        }
        case SoftmatchingType::__last__ : {
            break;
        }
    }
    P1.set_dirty();
    PT1.set_dirty();
    PX.set_dirty();
    std::cout << timer.pretty_report() << "\n";
}

void coherent_point_drift_base::update_P_full() {
    kernel_P.kernel_type = KernelType::gaussian;
    kernel_P.two_sigma_squared = 2 * sigma_squared;
    Matrix<kernel_precision, -1, -1> K = kernel_P.compute_kernel(Y.cast<kernel_precision>(),
                                                                 X.cast<kernel_precision>());
    kernel_precision c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * kernel_precision(M) /
                         kernel_precision(N);
    Vector<kernel_precision, -1> denominator = 1.0 / (K.colwise().sum().array() + c);
    K = K * denominator.asDiagonal();
    Map(PT1) = K.colwise().sum().cast<bcg_scalar_t>();
    Map(P1) = K.rowwise().sum().cast<bcg_scalar_t>();
    Map(PX) = (K * X.cast<kernel_precision>()).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

void coherent_point_drift_base::update_P_FGT() {
    kernel_P.kernel_type = KernelType::gaussian;
    kernel_P.two_sigma_squared = 2 * sigma_squared;
    Matrix<kernel_precision, -1, -1> K = kernel_P.compute_kernel(Y.cast<kernel_precision>(),
                                                                 X.cast<kernel_precision>());
    kernel_precision c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * kernel_precision(M) /
                         kernel_precision(N);
    Vector<kernel_precision, -1> a = 1.0 / (K.colwise().sum().array() + c);
    Map(PT1) = (1.0 - c * a.array()).cast<bcg_scalar_t>();
    Map(P1) = (K * a).cast<bcg_scalar_t>();
    Map(PX) = (K * (X.array().cast<kernel_precision>().colwise() * a.array()).matrix()).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

void coherent_point_drift_base::update_P_parallel(size_t parallel_grain_size) {
    VectorS<-1> denominator(N);
    kernel_precision two_sigma_squared = 2 * sigma_squared;
    kernel_precision c = std::pow(pi * two_sigma_squared, D / 2.0) * omega / (1.0 - omega) * kernel_precision(M) /
                         kernel_precision(N);
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) N, parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t n = range.begin(); n != range.end(); ++n) {
                    PT1[n] = 0;
                    for (long m = 0; m < M; ++m) {
                        bcg_scalar_t k_mn = std::exp(-(X.row(n) - Y.row(m)).squaredNorm() / two_sigma_squared);
                        PT1[n] += k_mn;
                    }
                    denominator[n] = PT1[n] + c;
                    PT1[n] /= denominator[n];
                }
            }
    );

    tbb::atomic<bcg_scalar_t> sum = 0;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) M, parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t m = range.begin(); m != range.end(); ++m) {
                    P1[m] = 0;
                    PX[m].setZero();
                    for (long n = 0; n < N; ++n) {
                        bcg_scalar_t k_mn = std::exp(-(X.row(n) - Y.row(m)).squaredNorm() / two_sigma_squared) /
                                            denominator[n];
                        P1[m] += k_mn;
                        PX[m] += k_mn * X.row(n);
                    }
                    sum = sum + P1[m];
                }
            }
    );
    N_P = sum;
}

void coherent_point_drift_base::update_P_nystroem() {
    kernel_P.kernel_type = KernelType::gaussian;
    kernel_P.two_sigma_squared = 2 * sigma_squared;
    num_samples = std::min<int>(num_samples, M + N);
    kernel_P.compute_nyström_approximation(Y.cast<kernel_precision>(), X.cast<kernel_precision>(), num_samples);

    if (debug_output) {
        std::cout << "error: " << kernel_P.approximation_error(Y.cast<kernel_precision>(), X.cast<kernel_precision>())
                  << "\n";
    }
    kernel_precision c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * kernel_precision(M) /
                         kernel_precision(N);

    Vector<kernel_precision, -1> denominator =
            1.0 / (((kernel_P.K_AV.colwise().sum() * kernel_P.K_VV_INV) * kernel_P.K_BV.transpose()).array() + c);
    Map(PT1) = (kernel_P.K_AV.colwise().sum() * kernel_P.K_VV_INV * kernel_P.K_BV.transpose() *
                denominator.asDiagonal()).cast<bcg_scalar_t>();
    Map(P1) = (kernel_P.K_AV * kernel_P.K_VV_INV * kernel_P.K_BV.transpose() * denominator).cast<bcg_scalar_t>();
    Map(PX) = (kernel_P.K_AV * kernel_P.K_VV_INV * kernel_P.K_BV.transpose() * denominator.asDiagonal() *
               X.cast<kernel_precision>()).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

void coherent_point_drift_base::update_P_nystroem_FGT() {
    kernel_P.kernel_type = KernelType::gaussian;
    kernel_P.two_sigma_squared = 2 * sigma_squared;
    num_samples = std::min<int>(num_samples, M + N);
    kernel_P.compute_nyström_approximation(Y.cast<kernel_precision>(), X.cast<kernel_precision>(), num_samples);

    if (debug_output) {
        std::cout << "error: " << kernel_P.approximation_error(Y.cast<kernel_precision>(), X.cast<kernel_precision>())
                  << "\n";
    }
    kernel_precision c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * kernel_precision(M) /
                         kernel_precision(N);
    Vector<kernel_precision, -1> a = 1.0 / (kernel_P.K_BV * (kernel_P.K_VV_INV * (kernel_P.K_AV.transpose() *
                                                                                  Vector<kernel_precision, -1>::Ones(
                                                                                          M))) +
                                            c * Vector<kernel_precision, -1>::Ones(N)).array();
    Map(PT1) = (1.0 - c * a.array()).cast<bcg_scalar_t>();
    Map(P1) = (kernel_P.K_AV * (kernel_P.K_VV_INV * (kernel_P.K_BV.transpose() * a))).cast<bcg_scalar_t>();
    Map(PX) = (kernel_P.K_AV * (kernel_P.K_VV_INV * (kernel_P.K_BV.transpose() *
                                                     (X.array().cast<kernel_precision>().colwise() *
                                                      a.array()).matrix()))).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

void coherent_point_drift_base::update_P_kdtree(size_t parallel_grain_size) {
    Matrix<kernel_precision, -1, -1> K = Matrix<kernel_precision, -1, -1>::Zero(M, N);
    Transform target_model_inverse = target_model->inverse();
    bcg_scalar_t radius = std::min<bcg_scalar_t>(0.15, 7 * std::sqrt(sigma_squared));
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) M, parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t m = range.begin(); m != range.end(); ++m) {
                    VectorS<3> Y_target_space = target_model_inverse * Y.row(m).transpose().homogeneous();
                    auto result = target_kdtree.query_radius(Y_target_space, radius);
                    if (result.indices.empty()) {
                        result = target_kdtree.query_knn(Y_target_space, 12);
                    }
                    for (unsigned long n : result.indices) {
                        K(m, n) = std::exp(
                                -(X.row(n).cast<kernel_precision>() -
                                  Y.row(m).cast<kernel_precision>()).squaredNorm() /
                                (2 * sigma_squared));
                    }
                }
            }
    );
    kernel_precision c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * kernel_precision(M) /
                         kernel_precision(N);
    Vector<kernel_precision, -1> a = 1.0 / ((K.transpose() * Vector<kernel_precision, -1>::Ones(M)) +
                                            c * Vector<kernel_precision, -1>::Ones(N)).array();
    Map(PT1) = (1.0 - c * a.array()).cast<bcg_scalar_t>();
    Map(P1) = (K * a).cast<bcg_scalar_t>();
    Map(PX) = (K * (X.array().cast<kernel_precision>().colwise() * a.array()).matrix()).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

MatrixS<-1, 3> coherent_point_drift_base::transform_source() {
    return ((source_model->linear() * MapConst(source_positions).transpose()).colwise() +
            source_model->translation()).transpose();
}

VectorS<3> coherent_point_drift_base::transform_source(size_t idx) {
    return source_model->linear() * source_positions[idx] + source_model->translation();
}

MatrixS<-1, 3> coherent_point_drift_base::transform_target() {
    return ((target_model->linear() * MapConst(target_positions).transpose()).colwise() +
            target_model->translation()).transpose();
}

VectorS<3> coherent_point_drift_base::transform_target(size_t idx) {
    return target_model->linear() * target_positions[idx] + target_model->translation();
}

void coherent_point_drift_rigid::init(vertex_container *source_vertices, Transform &source_model,
                                      vertex_container *target_vertices,
                                      Transform &target_model) {
    s = 1.0;
    t.setZero();
    R.setIdentity();
    coherent_point_drift_base::init(source_vertices, source_model, target_vertices, target_model);
}

void coherent_point_drift_rigid::compute_step() {
    update_P();
    VectorS<3> mean_x = X.transpose() * MapConst(PT1) / N_P;
    VectorS<3> mean_y = Y.transpose() * MapConst(P1) / N_P;
    MatrixS<-1, -1> A(MapConst(PX).transpose() * Y - N_P * mean_x * mean_y.transpose());
    R = project_on_so(A, true);
    bcg_scalar_t ARtrace = (A.transpose() * R).trace();
    s = ARtrace / ((Map(P1).array() * Y.rowwise().squaredNorm().array()).sum() - N_P * mean_y.squaredNorm());
    t = mean_x - s * R * mean_y;
    sigma_squared =
            ((MapConst(PT1).array() * X.rowwise().squaredNorm().array()).sum() -
             N_P * mean_x.squaredNorm() - s * ARtrace) /
            (N_P * D);
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
}

void coherent_point_drift_affine::init(vertex_container *source_vertices, Transform &source_model,
                                       vertex_container *target_vertices,
                                       Transform &target_model) {
    t.setZero();
    B.setIdentity();
    coherent_point_drift_base::init(source_vertices, source_model, target_vertices, target_model);
}

void coherent_point_drift_affine::compute_step() {
    update_P();
    VectorS<3> mean_x = X.transpose() * MapConst(PT1) / N_P;
    VectorS<3> mean_y = Y.transpose() * MapConst(P1) / N_P;
    MatrixS<-1, -1> A(MapConst(PX).transpose() * Y - N_P * mean_x * mean_y.transpose());
    B = A * (Y.transpose() * MapConst(P1).asDiagonal() * Y - N_P * mean_y * mean_y.transpose()).inverse();

    t = mean_x - B * mean_y;
    sigma_squared = ((MapConst(PT1).array() * X.rowwise().squaredNorm().array()).sum() -
                     N_P * mean_x.squaredNorm() -
                     (A * B.transpose()).trace()) / (N_P * D);
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
}

void coherent_point_drift_nonrigid::init(vertex_container *source_vertices, Transform &source_model,
                                         vertex_container *target_vertices,
                                         Transform &target_model) {

    deformation = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_deformation_vector", VectorS<3>::Zero());
    deformed = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_deformed", VectorS<3>::Zero());
    Map(deformation).setZero();
    coherent_point_drift_base::init(source_vertices, source_model, target_vertices, target_model);
    Map(deformed) = Y;
    kernel_G.two_sigma_squared = 2 * beta * beta;
    switch (coherence_type) {
        case CoherenceType::full : {
            G = kernel_G.compute_kernel(MapConst(source_positions).cast<kernel_precision>(),
                                        MapConst(source_positions).cast<kernel_precision>());
            break;
        }
        case CoherenceType::eigen : {
            assert(num_evals > 0);
            kernel_G.compute_eigen_approximation(MapConst(source_positions).cast<kernel_precision>(), num_evals);
            break;
        }
        case CoherenceType::__last__ : {
            break;
        }
    }
}

void coherent_point_drift_nonrigid::compute_step() {
    update_P();
    MatrixS<-1, -1> Y_undeformed = (*source_model *
                                    MapConst(source_positions).transpose().colwise().homogeneous()).transpose();
    switch (coherence_type) {
        case CoherenceType::full : {
            MatrixS<-1, -1> A = (MapConst(P1).asDiagonal() * G.cast<bcg_scalar_t>() +
                                 lambda * sigma_squared * MatrixS<-1, -1>::Identity(M, M));
            bcg_scalar_t levenberg_marquard = 0.1; //levenberg_marquard = 0.1 is small enough
            A += A.diagonal().asDiagonal() * levenberg_marquard;
            MatrixS<-1, -1> W = A.colPivHouseholderQr().solve(MapConst(PX) - MapConst(P1).asDiagonal() * Y_undeformed);
            Map(deformation) = G.cast<bcg_scalar_t>() * W;
            break;
        }
        case CoherenceType::eigen : {
            MatrixS<-1, -1> A_inv = (MapConst(P1).array() / (lambda * sigma_squared)).matrix().asDiagonal();
            MatrixS<-1, -1> C_inv = (1.0 / kernel_G.Evals.array().cast<bcg_scalar_t>()).matrix().asDiagonal();
            MatrixS<-1, -1> inv_inner = (C_inv + kernel_G.Evecs.cast<bcg_scalar_t>().transpose() * A_inv *
                                                 kernel_G.Evecs.cast<bcg_scalar_t>()).inverse();
            MatrixS<-1, -1> rhs =
                    A_inv * ((1.0 / MapConst(P1).array()).matrix().asDiagonal() * MapConst(PX) - Y_undeformed);
            MatrixS<-1, -1> W = rhs - A_inv * kernel_G.Evecs.cast<bcg_scalar_t>() * inv_inner *
                                      kernel_G.Evecs.cast<bcg_scalar_t>().transpose() * rhs;
            Map(deformation) = (kernel_G.Evecs.cast<bcg_scalar_t>() *
                                (kernel_G.Evals.cast<bcg_scalar_t>().asDiagonal() *
                                 (kernel_G.Evecs.cast<bcg_scalar_t>().transpose() * W)));
            break;
        }
        case CoherenceType::__last__ : {
            break;
        }
    }
    Map(deformation) = MapConst(deformation) * source_model->linear();
    Map(deformed) = MapConst(source_positions) + MapConst(deformation);
    Y = transform_source();
    sigma_squared =
            ((X.transpose() * MapConst(PT1).asDiagonal() * X).trace() -
             2.0 * (MapConst(PX).transpose() * Y).trace() +
             (Y.transpose() * MapConst(P1).asDiagonal() * Y).trace()) / (N_P * D);
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
    deformation.set_dirty();
    deformed.set_dirty();
}

MatrixS<-1, 3> coherent_point_drift_nonrigid::transform_source() {
    return ((MapConst(source_positions) + MapConst(deformation)) * source_model->linear().transpose()).rowwise() +
           source_model->translation().transpose();
}

VectorS<3> coherent_point_drift_nonrigid::transform_source(size_t idx) {
    return source_model->linear() * (source_positions[idx] + deformation[idx]) + source_model->translation();
}

void coherent_point_drift_nonrigid::accept_deformation() {
    Map(source_positions) += MapConst(deformation);
    source_positions.set_dirty();
    Map(deformation).setZero();
    deformation.set_dirty();
    Map(deformed) = Map(source_positions);
    deformed.set_dirty();
}

void coherent_point_drift_nonrigid2::init(vertex_container *source_vertices, Transform &source_model, vertex_container *target_vertices,
                                          Transform &target_model){
    deformation = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_deformation_vector", VectorS<3>::Zero());
    deformed = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_deformed", VectorS<3>::Zero());
    Map(deformation).setZero();
    coherent_point_drift_base::init(source_vertices, source_model, target_vertices, target_model);
    Map(deformed) = Y;
    kernel_G.two_sigma_squared = 2 * beta * beta;
    switch (coherence_type) {
        case CoherenceType::full : {
            G = kernel_G.compute_kernel(MapConst(source_positions).cast<kernel_precision>(),
                                        MapConst(source_positions).cast<kernel_precision>());
            break;
        }
        case CoherenceType::eigen : {
            assert(num_evals > 0);
            kernel_G.compute_eigen_approximation(MapConst(source_positions).cast<kernel_precision>(), num_evals);
            break;
        }
        case CoherenceType::__last__ : {
            break;
        }
    }
}

void coherent_point_drift_nonrigid2::accept_deformation(){
    Map(source_positions) += MapConst(deformation);
    source_positions.set_dirty();
    Map(deformation).setZero();
    deformation.set_dirty();
    Map(deformed) = Map(source_positions);
    deformed.set_dirty();
}

void coherent_point_drift_nonrigid2::compute_step(){
    update_P();

    VectorS<3> mean_x = X.transpose() * MapConst(PT1) / N_P;
    VectorS<3> mean_y = Y.transpose() * MapConst(P1) / N_P;
    MatrixS<-1, -1> A(MapConst(PX).transpose() * Y - N_P * mean_x * mean_y.transpose());
    R = project_on_so(A, true);
    bcg_scalar_t ARtrace = (A.transpose() * R).trace();
    s = ARtrace / ((MapConst(P1).array() * Y.rowwise().squaredNorm().array()).sum() - N_P * mean_y.squaredNorm());
    t = mean_x - s * R * mean_y;

    Transform model = Translation(t) * Rotation(R) * Scaling(VectorS<3>::Constant(3, s)) * *source_model;

    MatrixS<-1, -1> Y_undeformed = (model * MapConst(source_positions).transpose().colwise().homogeneous()).transpose();
    switch (coherence_type) {
        case CoherenceType::full : {
/*            MatrixS<-1, -1> A = (MapConst(P1).asDiagonal() * G.cast<bcg_scalar_t>() +
                                 lambda * sigma_squared * MatrixS<-1, -1>::Identity(M, M));*/
            MatrixS<-1, -1> A = (  MapConst(P1).asDiagonal() * G.cast<bcg_scalar_t>() +
                                 lambda * sigma_squared * MatrixS<-1, -1>::Identity(M, M));
/*            bcg_scalar_t levenberg_marquard = 0.1; //levenberg_marquard = 0.1 is small enough
            A += A.diagonal().asDiagonal() * levenberg_marquard;*/
            MatrixS<-1, -1> W = A.colPivHouseholderQr().solve(MapConst(PX) - MapConst(P1).asDiagonal() * Y_undeformed);
            Map(deformation) = G.cast<bcg_scalar_t>() * W;
            break;
        }
        case CoherenceType::eigen : {
            MatrixS<-1, -1> A_inv = (MapConst(P1).array() / (lambda * sigma_squared)).matrix().asDiagonal();
            MatrixS<-1, -1> C_inv = (1.0 / kernel_G.Evals.array().cast<bcg_scalar_t>()).matrix().asDiagonal();
            MatrixS<-1, -1> inv_inner = (C_inv + kernel_G.Evecs.cast<bcg_scalar_t>().transpose() * A_inv *
                                                 kernel_G.Evecs.cast<bcg_scalar_t>()).inverse();
            MatrixS<-1, -1> rhs =
                    A_inv * ((1.0 / MapConst(P1).array()).matrix().asDiagonal() * MapConst(PX) - Y_undeformed);
            MatrixS<-1, -1> W = rhs - A_inv * kernel_G.Evecs.cast<bcg_scalar_t>() * inv_inner *
                                      kernel_G.Evecs.cast<bcg_scalar_t>().transpose() * rhs;
            Map(deformation) = (kernel_G.Evecs.cast<bcg_scalar_t>() *
                                (kernel_G.Evals.cast<bcg_scalar_t>().asDiagonal() *
                                 (kernel_G.Evecs.cast<bcg_scalar_t>().transpose() * W)));
            break;
        }
        case CoherenceType::__last__ : {
            break;
        }
    }
    Map(deformation) = MapConst(deformation) * model.linear();
    Map(deformed) = MapConst(source_positions) + MapConst(deformation);
    Y =  (model * MapConst(deformed).transpose().colwise().homogeneous()).transpose();
    sigma_squared =
            ((X.transpose() * MapConst(PT1).asDiagonal() * X).trace() -
             2.0 * (MapConst(PX).transpose() * Y).trace() +
             (Y.transpose() * MapConst(P1).asDiagonal() * Y).trace()) / (N_P * D);
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
    deformation.set_dirty();
    deformed.set_dirty();
}

MatrixS<-1, 3> coherent_point_drift_nonrigid2::transform_source(){
    return ((MapConst(source_positions) + MapConst(deformation)) * source_model->linear().transpose()).rowwise() +
           source_model->translation().transpose();
}

VectorS<3> coherent_point_drift_nonrigid2::transform_source(size_t idx){
    return source_model->linear() * (source_positions[idx] + deformation[idx]) + source_model->translation();
}

void coherent_point_drift_bayes::init(vertex_container *source_vertices, Transform &source_model,
                                      vertex_container *target_vertices,
                                      Transform &target_model) {
    alpha = source_vertices->get_or_add<bcg_scalar_t, 1>("v_cpd_alpha");
    deformation = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_deformation_vector", VectorS<3>::Zero());
    deformed = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_deformed");
    p_out = target_vertices->get_or_add<bcg_scalar_t, 1>("v_cpd_p_out");
    residual = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_residual", VectorS<3>::Zero());

    Map(deformation).setZero();

    s = 1.0;
    t.setZero();
    R.setIdentity();
    coherent_point_drift_base::init(source_vertices, source_model, target_vertices, target_model);

    sigma_squared *= gamma;
    Sigma = MatrixS<-1, -1>::Identity(M, M);
    Map(alpha).array() = 1.0 / bcg_scalar_t(M);
    Map(deformed) = Y;
    Map(p_out).array() = 1.0 / bcg_scalar_t(N);//compute_target_volume();
    kernel_G.two_sigma_squared = 2 * beta * beta;
    switch (coherence_type) {
        case CoherenceType::full : {
            MatrixS<-1, -1> G = kernel_G.compute_kernel(MapConst(source_positions).cast<kernel_precision>(),
                                                        MapConst(source_positions).cast<kernel_precision>()).cast<bcg_scalar_t>();
            G.diagonal() *= 1.01;
            G_inv = G.inverse();
            if(debug_output){
                bcg_scalar_t error = (G_inv * G - MatrixS<-1, -1>::Identity(M, M)).mean();
                std::cout << "error G_inv: " << error << "\n";
            }
            break;
        }
        case CoherenceType::eigen : {
            kernel_G.compute_eigen_approximation(Y.cast<kernel_precision>(), num_evals);
            break;
        }
        case CoherenceType::__last__ : {
            break;
        }
    }
}

bcg_scalar_t coherent_point_drift_bayes::compute_target_volume() const {
    VectorS<-1> diff = MapConst(target_positions).colwise().maxCoeff().array() -
                       MapConst(target_positions).colwise().minCoeff().array();
    bcg_scalar_t result = 1;
    for (size_t i = 0; i < D; ++i) {
        if (diff[i] > scalar_eps) {
            result *= diff[i] * bcg_scalar_t(N + 1) / bcg_scalar_t(N);
        }
    }
    //return (diff * (N + 1) / bcg_scalar_t(N)).prod();
    assert(result >= scalar_eps);
    return result;
}

double digamma(double x) {
    double r, f, t;
    r = 0;
    while (x <= 5) {
        r -= 1 / x;
        x += 1;
    }
    f = 1 / (x * x);
    t = f * (-1 / 12.0 + f * (1 / 120.0 + f * (-1 / 252.0 + f * (1 / 240.0 + f * (-1 / 132.0
                                                                                  + f * (691 / 32760.0 + f *
                                                                                                         (-1 / 12.0 +
                                                                                                          f * 3617 /
                                                                                                          8160.0)))))));
    return r + log(x) - 0.5 / x + t;
}

void coherent_point_drift_bayes::compute_step() {
    update_P();
    Map(residual) = (1.0 / Map(P1).array()).matrix().asDiagonal() * MapConst(PX);
    switch (coherence_type) {
        case CoherenceType::full : {
            Sigma = (lambda * G_inv +
                    s * s / sigma_squared * MatrixS<-1, -1>::Identity(M, M) * MapConst(P1).asDiagonal()).inverse();
            break;
        }
        case CoherenceType::eigen : {
            //TODO Idea here ist to scale the entries of eigen vectors depending on registration error... locally...
            //MatrixS<-1, -1> Evecs = (1.0 / (-Map(deformation).rowwise().norm()).array().exp()).matrix().asDiagonal() * kernel_G.Evecs;
            //MatrixS<-1, -1> Evecs = (1.0 / (-Map(P1)).array().exp()).matrix().asDiagonal() * kernel_G.Evecs;
            /*MatrixS<-1, -1> Evecs = (1.0 / (-Map(P1)).array().exp().max((-Map(deformation).rowwise().norm()).array().exp())).matrix().asDiagonal() * kernel_G.Evecs;
            MatrixS<-1, -1> S = Evecs.cast<bcg_scalar_t>().transpose() * MapConst(P1).asDiagonal() *
                                Evecs.cast<bcg_scalar_t>();
            MatrixS<-1, -1> ID = MatrixS<-1, -1>::Identity(kernel_G.Evals.size(), kernel_G.Evals.size());
            Sigma = (Evecs.cast<bcg_scalar_t>() * kernel_G.Evals.cast<bcg_scalar_t>().asDiagonal() *
                     (ID - S * (ID * (lambda * sigma_squared /
                                      (s * s * kernel_G.Evals.cast<bcg_scalar_t>().array())).matrix().asDiagonal() +
                                S).inverse()) *
                     Evecs.cast<bcg_scalar_t>().transpose()) / lambda;*/


            MatrixS<-1, -1> S = kernel_G.Evecs.cast<bcg_scalar_t>().transpose() * MapConst(P1).asDiagonal() *
                                kernel_G.Evecs.cast<bcg_scalar_t>();
            MatrixS<-1, -1> ID = MatrixS<-1, -1>::Identity(kernel_G.Evals.size(), kernel_G.Evals.size());
            Sigma = (kernel_G.Evecs.cast<bcg_scalar_t>() * kernel_G.Evals.cast<bcg_scalar_t>().asDiagonal() *
                     (ID - S * (ID * (lambda * sigma_squared /
                                      (s * s * kernel_G.Evals.cast<bcg_scalar_t>().array())).matrix().asDiagonal() +
                                S).inverse()) *
                     kernel_G.Evecs.cast<bcg_scalar_t>().transpose()) / lambda;
            break;
        }
        case CoherenceType::__last__ : {
            break;
        }
    }

    Y = (MapConst(source_positions) * backup_source_model.linear().transpose()).rowwise() +
        backup_source_model.translation().transpose(); // Maybe here again backup?
    MatrixS<-1, 3> V =
            s * s / sigma_squared * Sigma * MapConst(P1).asDiagonal() * (transform_inverse(MapConst(residual)) - Y);
    Y += V;
    Map(deformation) = V * backup_source_model.linear();

    for (size_t m = 0; m < M; ++m) {
        alpha[m] = std::exp(digamma(kappa + P1[m]) - digamma(kappa * M + N_P));
    }

    bcg_scalar_t sigma_squared_bar = (Sigma.diagonal().transpose() * MapConst(P1))[0] / N_P;
    VectorS<3> mean_x = MapConst(residual).transpose() * MapConst(P1) / N_P;
    VectorS<3> mean_y = Y.transpose() * MapConst(P1) / N_P;
    MatrixS<-1, -1> Y_hat = Y.rowwise() - mean_y.transpose();
    MatrixS<-1, -1> S_XY =
            (MapConst(residual).rowwise() - mean_x.transpose()).transpose() * MapConst(P1).asDiagonal() * Y_hat / N_P;
    MatrixS<-1, -1> S_YY = Y_hat.transpose() * MapConst(P1).asDiagonal() * Y_hat / N_P +
                           sigma_squared_bar * MatrixS<-1, -1>::Identity(D, D);
    R = project_on_so(S_XY, true);
    s = (S_XY.transpose() * R).trace() / S_YY.trace();
    t = mean_x - s * R * mean_y;

    Y = (s * Y * R.transpose()).rowwise() + t.transpose();
    sigma_squared = ((X.transpose() * MapConst(PT1).asDiagonal() * X).trace() -
                     2 * (MapConst(PX).transpose() * Y).trace() +
                     (Y.transpose() * MapConst(P1).asDiagonal() * Y).trace()) /
                    (N_P * D) + s * s * sigma_squared_bar;
    Map(deformed) = MapConst(source_positions) + MapConst(deformation);
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
    deformation.set_dirty();
    deformed.set_dirty();
}


void coherent_point_drift_bayes::update_P_full() {
    kernel_P.kernel_type = KernelType::gaussian;
    kernel_P.two_sigma_squared = 2 * sigma_squared;

    Vector<kernel_precision, -1> weight =
            (-s * s / kernel_P.two_sigma_squared * Sigma.cast<kernel_precision>().diagonal() * D).array().exp() *
            MapConst(alpha).cast<kernel_precision>().array() *
            (1.0 - omega);
    Matrix<kernel_precision, -1, -1> K = weight.asDiagonal() *
                                         kernel_P.compute_kernel(Y.cast<kernel_precision>(),
                                                                 X.cast<kernel_precision>()) /
                                         std::pow(pi * kernel_P.two_sigma_squared, D / 2.0);
    Vector<kernel_precision, -1> c = omega * Map(p_out).cast<kernel_precision>();
    Vector<kernel_precision, -1> denominator = 1.0 / (K.colwise().sum().transpose() + c).array();
    K = K * denominator.asDiagonal();
    Map(PT1) = K.colwise().sum().cast<bcg_scalar_t>();
    Map(P1) = K.rowwise().sum().cast<bcg_scalar_t>();
    Map(PX) = (K * X.cast<kernel_precision>()).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

void coherent_point_drift_bayes::update_P_FGT() {
    kernel_P.kernel_type = KernelType::gaussian;
    kernel_P.two_sigma_squared = 2 * sigma_squared;

    Vector<kernel_precision, -1> weight =
            (-s * s / kernel_P.two_sigma_squared * Sigma.cast<kernel_precision>().diagonal() * D).array().exp() *
            MapConst(alpha).cast<kernel_precision>().array();
    Matrix<kernel_precision, -1, -1> K = kernel_P.compute_kernel(Y.cast<kernel_precision>(),
                                                                 X.cast<kernel_precision>());
    Vector<kernel_precision, -1> c =
            std::pow(pi * kernel_P.two_sigma_squared, D / 2.0) * omega / (1 - omega) *
            MapConst(p_out).cast<kernel_precision>();

    Vector<kernel_precision, -1> a = 1.0 / (K.transpose() * weight + c).array();
    Map(PT1) = (1.0 - c.array() * a.array()).cast<bcg_scalar_t>();
    Map(P1) = (weight.asDiagonal() * K * a).cast<bcg_scalar_t>();
    Map(PX) = (weight.asDiagonal() * K *
               (X.array().cast<kernel_precision>().colwise() * a.array()).matrix()).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

void coherent_point_drift_bayes::update_P_parallel(size_t parallel_grain_size) {
    VectorS<-1> denominator(N);
    kernel_precision two_sigma_squared = 2 * sigma_squared;
    Vector<kernel_precision, -1> weight =
            (-s * s / two_sigma_squared * Sigma.cast<kernel_precision>().diagonal() * D).array().exp() *
            MapConst(alpha).cast<kernel_precision>().array() *
            (1.0 - omega) / std::pow(pi * two_sigma_squared, D / 2.0);

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) N, parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t n = range.begin(); n != range.end(); ++n) {
                    PT1[n] = 0;
                    for (long m = 0; m < M; ++m) {
                        bcg_scalar_t k_mn =
                                std::exp(-(X.row(n) - Y.row(m)).squaredNorm() / two_sigma_squared) * weight[m];
                        PT1[n] += k_mn;
                    }
                    denominator[n] = PT1[n] + omega * p_out[n];
                    PT1[n] /= denominator[n];
                }
            }
    );

    tbb::atomic<bcg_scalar_t> sum = 0;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) M, parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t m = range.begin(); m != range.end(); ++m) {
                    P1[m] = 0;
                    PX[m].setZero();
                    for (long n = 0; n < N; ++n) {
                        bcg_scalar_t k_mn =
                                std::exp(-(X.row(n) - Y.row(m)).squaredNorm() / two_sigma_squared) * weight[m] /
                                denominator[n];
                        P1[m] += k_mn;
                        PX[m] += k_mn * X.row(n);
                    }
                    sum = sum + P1[m];
                }
            }
    );
    N_P = sum;
}

void coherent_point_drift_bayes::update_P_nystroem() {
    kernel_P.kernel_type = KernelType::gaussian;
    kernel_P.two_sigma_squared = 2 * sigma_squared;
    num_samples = std::min<int>(num_samples, M + N);
    kernel_P.compute_nyström_approximation(Y.cast<kernel_precision>(), X.cast<kernel_precision>(), num_samples);

    if (debug_output) {
        std::cout << "error: "
                  << kernel_P.approximation_error(Y.cast<kernel_precision>(), X.cast<kernel_precision>())
                  << "\n";
    }
    Vector<kernel_precision, -1> weight =
            (-s * s / kernel_P.two_sigma_squared * Sigma.cast<kernel_precision>().diagonal() * D).array().exp() *
            MapConst(alpha).cast<kernel_precision>().array() *
            (1.0 - omega) / std::pow(pi * kernel_P.two_sigma_squared, D / 2.0);

    Vector<kernel_precision, -1> c = omega * Map(p_out).cast<kernel_precision>();

    Vector<kernel_precision, -1> denominator =
            1.0 / ((kernel_P.K_BV * kernel_P.K_VV_INV * kernel_P.K_AV.transpose() * weight) + c).array();
    Map(PT1) = ((weight.asDiagonal() * kernel_P.K_AV).colwise().sum() * kernel_P.K_VV_INV * kernel_P.K_BV.transpose() *
                denominator.asDiagonal()).cast<bcg_scalar_t>();
    Map(P1) = (weight.asDiagonal() * kernel_P.K_AV * kernel_P.K_VV_INV * kernel_P.K_BV.transpose() *
               denominator).cast<bcg_scalar_t>();
    Map(PX) = (weight.asDiagonal() * kernel_P.K_AV * kernel_P.K_VV_INV * kernel_P.K_BV.transpose() *
               denominator.asDiagonal() *
               X.cast<kernel_precision>()).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

void coherent_point_drift_bayes::update_P_nystroem_FGT() {
    kernel_P.kernel_type = KernelType::gaussian;
    kernel_P.two_sigma_squared = 2 * sigma_squared;
    num_samples = std::min<int>(num_samples, M + N);
    kernel_P.compute_nyström_approximation(Y.cast<kernel_precision>(), X.cast<kernel_precision>(), num_samples);

    if (debug_output) {
        std::cout << "error: " << kernel_P.approximation_error(Y.cast<kernel_precision>(), X.cast<kernel_precision>())
                  << "\n";
    }

    Vector<kernel_precision, -1> weight =
            (-s * s / kernel_P.two_sigma_squared * Sigma.cast<kernel_precision>().diagonal() * D).array().exp() *
            MapConst(alpha).cast<kernel_precision>().array();

    Vector<kernel_precision, -1> c =
            std::pow(pi * kernel_P.two_sigma_squared, D / 2.0) * omega * MapConst(p_out).cast<kernel_precision>() /
            (1.0 - omega);

    Vector<kernel_precision, -1> a =
            1.0 / (kernel_P.K_BV * kernel_P.K_VV_INV * kernel_P.K_AV.transpose() * weight + c).array();
    Map(PT1) = (1.0 - c.array() * a.array()).cast<bcg_scalar_t>();
    Map(P1) = (weight.asDiagonal() * kernel_P.K_AV *
               (kernel_P.K_VV_INV * (kernel_P.K_BV.transpose() * a))).cast<bcg_scalar_t>();
    Map(PX) = (weight.asDiagonal() * kernel_P.K_AV * (kernel_P.K_VV_INV * (kernel_P.K_BV.transpose() *
                                                                           (X.array().cast<kernel_precision>().colwise() *
                                                                            a.array()).matrix()))).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

void coherent_point_drift_bayes::update_P_kdtree(size_t parallel_grain_size) {
    Matrix<kernel_precision, -1, -1> K = Matrix<kernel_precision, -1, -1>::Zero(M, N);

    kernel_precision two_sigma_squared = 2 * sigma_squared;

    Vector<kernel_precision, -1> weight =
            (-s * s / two_sigma_squared * Sigma.cast<kernel_precision>().diagonal() * D).array().exp() *
            MapConst(alpha).cast<kernel_precision>().array();
    Vector<kernel_precision, -1> c =
            std::pow(pi * two_sigma_squared, D / 2.0) * omega * MapConst(p_out).cast<kernel_precision>() /
            (1.0 - omega);

    bcg_scalar_t radius = std::min<bcg_scalar_t>(0.15, 7 * std::sqrt(sigma_squared));

    Transform target_model_inverse = target_model->inverse();
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) M, parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t m = range.begin(); m != range.end(); ++m) {
                    VectorS<3> Y_target_space = target_model_inverse * Y.row(m).transpose().homogeneous();
                    auto result = target_kdtree.query_radius(Y_target_space, radius);
                    if (result.indices.empty()) {
                        result = target_kdtree.query_knn(Y_target_space, 12);
                    }
                    for (unsigned long n : result.indices) {
                        K(m, n) = std::exp(
                                -(X.row(n).cast<kernel_precision>() -
                                  Y.row(m).cast<kernel_precision>()).squaredNorm() / two_sigma_squared);
                    }
                }
            }
    );

    Vector<kernel_precision, -1> a = 1.0 / (K.transpose() * weight + c).array();
    Map(PT1) = (1.0 - c.array() * a.array()).cast<bcg_scalar_t>();
    Map(P1) = (weight.asDiagonal() * K * a).cast<bcg_scalar_t>();
    Map(PX) = (weight.asDiagonal() * K *
               (X.array().cast<kernel_precision>().colwise() * a.array()).matrix()).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

MatrixS<-1, 3> coherent_point_drift_bayes::transform_inverse(const MatrixS<-1, 3> &residual) {
    return (residual.rowwise() - t.transpose()) * R / s;
}

MatrixS<-1, 3> coherent_point_drift_bayes::transform_source() {
    return ((MapConst(source_positions) + MapConst(deformation)) * source_model->linear().transpose()).rowwise() +
           source_model->translation().transpose();
}

VectorS<3> coherent_point_drift_bayes::transform_source(size_t idx) {
    return (source_positions[idx] + deformation[idx]).transpose() * source_model->linear().transpose() +
           source_model->translation().transpose();
}


}