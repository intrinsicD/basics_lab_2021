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


void coherent_point_drift_base2::init(vertex_container *source_vertices, Transform &source_model,
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
    OnesN = Vector<kernel_precision, -1>::Ones(N);
    OnesM = Vector<kernel_precision, -1>::Ones(M);
    X = transform_target();
    Y = transform_source();
    sigma_squared = pairwise_distance_squared(X, Y).sum() / (M * N * D);
}

void coherent_point_drift_base2::reset() {
    *source_model = backup_source_model;
    *target_model = backup_target_model;
    X = transform_target();
    Y = transform_source();
    sigma_squared = pairwise_distance_squared(X, Y).sum();
}

void coherent_point_drift_base2::update_P() {
    Timer timer;
    if (std::sqrt(sigma_squared) < kdtree_sigma_threshold) {
        type_P = Type_P::kdtree;
    }
    switch (type_P) {
        case Type_P::full : {
            update_P_full();
            std::cout << "P_full: ";
            break;
        }
        case Type_P::full_FGT : {
            update_P_FGT();
            std::cout << "P_full_FGT: ";
            break;
        }
        case Type_P::parallel : {
            update_P_parallel();
            std::cout << "P_parallel: ";
            break;
        }
        case Type_P::nystroem : {
            update_P_nystroem();
            std::cout << "P_nystroem: ";
            break;
        }
        case Type_P::nystroem_FGT : {
            update_P_nystroem_FGT();
            std::cout << "P_nystroem_FGT: ";
            break;
        }
        case Type_P::kdtree : {
            update_P_kdtree();
            std::cout << "P_kdtree: ";
            break;
        }
    }
    std::cout << timer.pretty_report() << "\n";
}

void coherent_point_drift_base2::update_P_full() {
    kernel_matrix<kernel_precision> kernel_P;
    kernel_P.kernel_type = KernelType::gaussian;
    kernel_P.two_sigma_squared = 2 * sigma_squared;
    Y = transform_source();
    Matrix<kernel_precision, -1, -1> K = kernel_P.compute_kernel(Y.cast<kernel_precision>(),
                                                                 X.cast<kernel_precision>());
    kernel_precision c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * kernel_precision(M) /
                         kernel_precision(N);
    Map(PT1) = (K * OnesN).cast<bcg_scalar_t>();
    Vector<kernel_precision, -1> denominator = 1.0 / (MapConst(PT1).array().cast<kernel_precision>() + c);
    Map(PT1) = (K * denominator.asDiagonal() * OnesN).cast<bcg_scalar_t>();
    Map(P1) = (denominator.asDiagonal() * K.transpose() * OnesM).cast<bcg_scalar_t>();
    Map(PX) = (K * denominator.asDiagonal() * X.cast<kernel_precision>()).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

void coherent_point_drift_base2::update_P_FGT() {
    kernel_matrix<kernel_precision> kernel_P;
    kernel_P.kernel_type = KernelType::gaussian;
    kernel_P.two_sigma_squared = 2 * sigma_squared;
    Y = transform_source();
    Matrix<kernel_precision, -1, -1> K = kernel_P.compute_kernel(Y.cast<kernel_precision>(),
                                                                 X.cast<kernel_precision>());
    kernel_precision c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * kernel_precision(M) /
                         kernel_precision(N);
    Vector<kernel_precision, -1> a = 1.0 / ((K.transpose() * OnesM) + c * OnesN).array();
    Map(PT1) = (OnesN - c * a).cast<bcg_scalar_t>();
    Map(P1) = (K * a).cast<bcg_scalar_t>();
    Map(PX) = (K * (X.array().cast<kernel_precision>().colwise() * a.array()).matrix()).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

void coherent_point_drift_base2::update_P_parallel(size_t parallel_grain_size) {
    VectorS<-1> denominator(N);
    kernel_precision c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * kernel_precision(M) /
                         kernel_precision(N);
    Y = transform_source();
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) N, parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t n = range.begin(); n != range.end(); ++n) {
                    PT1[n] = 0;
                    for (long m = 0; m < M; ++m) {
                        bcg_scalar_t k_mn = std::exp(-(X.row(n) - Y.row(m)).squaredNorm() / (2 * sigma_squared));
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
                        bcg_scalar_t k_mn = std::exp(-(X.row(n) - Y.row(m)).squaredNorm() / (2 * sigma_squared)) /
                                            denominator[n];
                        P1[m] += k_mn;
                        PX[m] += k_mn * X;
                    }
                    sum = sum + P1[m];
                }
            }
    );
    N_P = sum;
}

void coherent_point_drift_base2::update_P_nystroem() {
    using kernel_precision = double;
    kernel_matrix<kernel_precision> kernel_P;
    kernel_P.kernel_type = KernelType::gaussian;
    kernel_P.two_sigma_squared = 2 * sigma_squared;
    num_samples = std::min<int>(num_samples, M + N);
    Y = transform_source();
    kernel_P.compute_nyström_approximation(Y.cast<kernel_precision>(), X.cast<kernel_precision>(), num_samples);

    if (debug_output) {
        std::cout << "error: " << kernel_P.approximation_error(Y.cast<kernel_precision>(), X.cast<kernel_precision>())
                  << "\n";
    }
    kernel_precision c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * kernel_precision(M) /
                         kernel_precision(N);
    Map(PT1) = (kernel_P.K_AV * (kernel_P.K_VV_INV * (kernel_P.K_BV.transpose() * OnesN))).cast<bcg_scalar_t>();
    Vector<kernel_precision, -1> denominator = 1.0 / (MapConst(PT1).array().cast<kernel_precision>() + c);
    Map(PT1) = (kernel_P.K_AV * (kernel_P.K_VV_INV *
                                 (kernel_P.K_BV.transpose() * denominator.asDiagonal() * OnesN))).cast<bcg_scalar_t>();
    Map(P1) = (denominator.asDiagonal() *
               (kernel_P.K_BV * (kernel_P.K_VV_INV * (kernel_P.K_AV.transpose() * OnesM)))).cast<bcg_scalar_t>();
    Map(PX) = ((kernel_P.K_AV * (kernel_P.K_VV_INV * (kernel_P.K_BV.transpose() * denominator.asDiagonal() *
                                                      X.cast<kernel_precision>())))).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

void coherent_point_drift_base2::update_P_nystroem_FGT() {
    using kernel_precision = double;
    kernel_matrix<kernel_precision> kernel_P;
    kernel_P.kernel_type = KernelType::gaussian;
    kernel_P.two_sigma_squared = 2 * sigma_squared;
    num_samples = std::min<int>(num_samples, M + N);
    Y = transform_source();
    kernel_P.compute_nyström_approximation(Y.cast<kernel_precision>(), X.cast<kernel_precision>(), num_samples);

    if (debug_output) {
        std::cout << "error: " << kernel_P.approximation_error(Y, X) << "\n";
    }
    kernel_precision c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * kernel_precision(M) /
                         kernel_precision(N);
    Vector<kernel_precision, -1> a = 1.0 / (kernel_P.K_BV * (kernel_P.K_VV_INV * (kernel_P.K_AV.transpose() * OnesM)) +
                                            c * OnesN).array();
    Map(PT1) = (OnesN - c * a).cast<bcg_scalar_t>();
    Map(P1) = (kernel_P.K_AV * (kernel_P.K_VV_INV * (kernel_P.K_BV.transpose() * a))).cast<bcg_scalar_t>();
    Map(PX) = (kernel_P.K_AV * (kernel_P.K_VV_INV * (kernel_P.K_BV.transpose() *
                                                     (X.array().cast<kernel_precision>().colwise() *
                                                      a.array()).matrix()))).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

void coherent_point_drift_base2::update_P_kdtree(size_t parallel_grain_size) {
    Matrix<kernel_precision, -1, -1> K = Matrix<kernel_precision, -1, -1>::Zero(M, N);
    Y = transform_source();
    Transform target_model_inverse = target_model->inverse();
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) M, parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t m = range.begin(); m != range.end(); ++m) {
                    VectorS<3> Y_target_space = target_model_inverse * Y.row(m);
                    auto result = target_kdtree.query_radius(Y_target_space, 7 * sigma_squared);
                    if (result.indices.empty()) {
                        result = target_kdtree.query_knn(Y_target_space, 12);
                    }
                    for (unsigned long n : result.indices) {
                        K(m, n) = std::exp(
                                -(X.row(n).cast<kernel_precision>() -
                                  Y_target_space.cast<kernel_precision>()).squaredNorm() /
                                (2 * sigma_squared));
                    }
                }
            }
    );
    kernel_precision c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * kernel_precision(M) /
                         kernel_precision(N);
    Vector<kernel_precision, -1> a = 1.0 / ((K.transpose() * OnesM) + c * OnesN).array();
    Map(PT1) = (OnesN - c * a).cast<bcg_scalar_t>();
    Map(P1) = (K * a).cast<bcg_scalar_t>();
    Map(PX) = (K * (X.array().cast<kernel_precision>().colwise() * a.array()).matrix()).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

MatrixS<-1, 3> coherent_point_drift_base2::transform_source() {
    return ((source_model->linear() * MapConst(source_positions).transpose()).colwise() +
            source_model->translation()).transpose();
}

VectorS<3> coherent_point_drift_base2::transform_source(size_t idx) {
    return source_model->linear() * source_positions[idx] + source_model->translation();
}

MatrixS<-1, 3> coherent_point_drift_base2::transform_target() {
    return ((target_model->linear() * MapConst(target_positions).transpose()).colwise() +
            target_model->translation()).transpose();
}

VectorS<3> coherent_point_drift_base2::transform_target(size_t idx) {
    return target_model->linear() * target_positions[idx] + target_model->translation();
}

void coherent_point_drift_rigid::init(vertex_container *source_vertices, Transform &source_model,
                                      vertex_container *target_vertices,
                                      Transform &target_model) {
    s = 1.0;
    t.setZero();
    R.setIdentity();
    coherent_point_drift_base2::init(source_vertices, source_model, target_vertices, target_model);
}

void coherent_point_drift_rigid::compute_step() {
    update_P();
    VectorS<3> mean_x = X.transpose() * PT1 / N_P;
    VectorS<3> mean_y = Y.transpose() * P1 / N_P;
    MatrixS<-1, -1> A(MapConst(PX).transpose() * Y - N_P * mean_x * mean_y.transpose());
    R = project_on_so(A, true);
    bcg_scalar_t ARtrace = (A.transpose() * R).trace();
    s = ARtrace / ((Map(P1).array() * Y.rowwise().squaredNorm().array()).sum() - N_P * mean_y.squaredNorm());
    t = mean_x - s * R * mean_y;
    sigma_squared =
            ((Map(PT1).array() * X.rowwise().squaredNorm().array()).sum() -
             N_P * mean_x.squaredNorm() - s * ARtrace) /
            (N_P * D);
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
    *source_model = Translation(t) * Rotation(R) * Scaling(VectorS<3>::Constant(s)) * *source_model;
}

void coherent_point_drift_affine::init(vertex_container *source_vertices, Transform &source_model,
                                       vertex_container *target_vertices,
                                       Transform &target_model) {
    t.setZero();
    B.setIdentity();
    coherent_point_drift_base2::init(source_vertices, source_model, target_vertices, target_model);
}

void coherent_point_drift_affine::compute_step() {
    update_P();
    VectorS<3> mean_x = X.transpose() * PT1 / N_P;
    VectorS<3> mean_y = Y.transpose() * P1 / N_P;
    MatrixS<-1, -1> A(Map(PX).transpose() * Y - N_P * mean_x * mean_y.transpose());
    B = A * (Y.transpose() * Map(P1).asDiagonal() * Y - N_P * mean_y * mean_y.transpose()).inverse();

    t = mean_x - B * mean_y;
    sigma_squared = ((Map(PT1).array() * X.rowwise().squaredNorm().array()).sum() - N_P * mean_x.squaredNorm() -
                     (A * B.transpose()).trace()) /
                    (N_P * D);
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
    *source_model = Translation(t) * Rotation(B) * *source_model;
}

void coherent_point_drift_nonrigid::init(vertex_container *source_vertices, Transform &source_model,
                                         vertex_container *target_vertices,
                                         Transform &target_model) {
    coherent_point_drift_base2::init(source_vertices, source_model, target_vertices, target_model);

    deformation = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_deformation_vector", VectorS<3>::Zero());
    deformed = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_deformed", VectorS<3>::Zero());
    Map(deformation).setZero();
    Map(deformed) = Y;

    switch (coherence_type) {
        case CoherenceType::full : {
            G = kernel_G.compute_kernel(Y.cast<kernel_precision>(), Y.cast<kernel_precision>());
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

void coherent_point_drift_nonrigid::reset() {
    Map(deformation).setZero();
    coherent_point_drift_base2::reset();
    Map(deformed) = Y;
}

void coherent_point_drift_nonrigid::compute_step() {
    update_P();
    MatrixS<-1, 3> V;
    switch (coherence_type) {
        case CoherenceType::full : {
            MatrixS<-1, -1> A = (MapConst(P1).asDiagonal() * G.cast<bcg_scalar_t>() +
                                 lambda * sigma_squared * MatrixS<-1, -1>::Identity(M, M));
            bcg_scalar_t levenberg_marquard = 0.1; //levenberg_marquard = 0.1 is small enough
            A += A.diagonal().asDiagonal() * levenberg_marquard;
            MatrixS<-1, -1> W = A.colPivHouseholderQr().solve(MapConst(PX) - MapConst(P1).asDiagonal() * Y);
            V = G.cast<bcg_scalar_t>() * W;
            break;
        }
        case CoherenceType::eigen : {
            MatrixS<-1, -1> A_inv = (MapConst(P1).array() / (lambda * sigma_squared)).matrix().asDiagonal();
            MatrixS<-1, -1> C_inv = (1.0 / kernel_G.Evals.array().cast<bcg_scalar_t>()).matrix().asDiagonal();
            MatrixS<-1, -1> inv_inner = (C_inv + kernel_G.Evecs.cast<bcg_scalar_t>().transpose() * A_inv *
                                                 kernel_G.Evecs.cast<bcg_scalar_t>()).inverse();
            MatrixS<-1, -1> rhs = A_inv * ((1.0 / MapConst(P1).array()).matrix().asDiagonal() * MapConst(PX) - Y);
            MatrixS<-1, -1> W = rhs - A_inv * kernel_G.Evecs.cast<bcg_scalar_t>() * inv_inner *
                                      kernel_G.Evecs.cast<bcg_scalar_t>().transpose() * rhs;
            V = (kernel_G.Evecs.cast<bcg_scalar_t>() * (kernel_G.Evals.cast<bcg_scalar_t>().asDiagonal() *
                                                        (kernel_G.Evecs.cast<bcg_scalar_t>().transpose() * W)));
            break;
        }
        case CoherenceType::__last__ : {
            break;
        }
    }
    Map(deformation) = V * source_model->linear();
    Map(deformed) = Y + V;
    sigma_squared =
            ((X.transpose() * MapConst(PT1).asDiagonal() * X).trace() -
             2.0 * (MapConst(PX).transpose() * Map(deformed)).trace() +
             (Map(deformed).transpose() * MapConst(P1).asDiagonal() * Map(deformed)).trace()) / (N_P * D);
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
    deformation.set_dirty();
}

MatrixS<-1, 3> coherent_point_drift_nonrigid::transform_source() {
    return ((source_model->linear() * (MapConst(source_positions) + MapConst(deformation)).transpose()).colwise() +
            source_model->translation()).transpose();
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

void coherent_point_drift_bayes::init(vertex_container *source_vertices, Transform &source_model,
                                      vertex_container *target_vertices,
                                      Transform &target_model) {
    coherent_point_drift_base2::init(source_vertices, source_model, target_vertices, target_model);

    alpha = source_vertices->get_or_add<bcg_scalar_t, 1>("v_cpd_alpha");
    deformation = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_deformation_vector", VectorS<3>::Zero());
    deformed = source_vertices->get_or_add<VectorS<3>, 3>("v_cpd_deformed");
    p_out = target_vertices->get_or_add<bcg_scalar_t, 1>("v_cpd_p_out");

    sigma_squared *= gamma;
    Sigma = MatrixS<-1, -1>::Identity(M, M);
    s = 1.0;
    t.setZero();
    R.setIdentity();
    Map(alpha) = VectorS<-1>::Constant(M, 1.0 / bcg_scalar_t(M));
    Map(deformation).setZero();
    Map(deformed) = Y;
    Map(p_out).array() = 1.0 / compute_target_volume();

    switch (coherence_type) {
        case CoherenceType::full : {
            G_inv = kernel_G.compute_kernel(Y.cast<kernel_precision>(),
                                            Y.cast<kernel_precision>()).inverse().cast<bcg_scalar_t>();
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

void coherent_point_drift_bayes::reset() {
    Sigma = MatrixS<-1, -1>::Identity(M, M);
    s = 1.0;
    t.setZero();
    R.setIdentity();
    Map(alpha) = VectorS<-1>::Constant(M, 1.0 / bcg_scalar_t(M));
    Map(deformation).setZero();
    Map(p_out).array() = 1.0 / compute_target_volume();
    coherent_point_drift_base2::reset();
    sigma_squared *= gamma;
    Map(deformed) = Y;
}

bcg_scalar_t coherent_point_drift_bayes::compute_target_volume() const {
    return ((Map(target_positions).colwise().maxCoeff() - Map(target_positions).colwise().minCoeff()) * (N + 1) /
            bcg_scalar_t(N)).prod();
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
    Map(PX) = (1.0 / Map(P1).array()).matrix().asDiagonal() * MapConst(PX);
    switch (coherence_type) {
        case CoherenceType::full : {
            Sigma = (lambda * G_inv +
                     s * s / sigma_squared * MatrixS<-1, -1>::Identity(M, M) * MapConst(P1).asDiagonal()).inverse();
            break;
        }
        case CoherenceType::eigen : {
            MatrixS<-1, -1> S = kernel_G.Evecs.cast<bcg_scalar_t>().transpose() * MapConst(P1).asDiagonal() *
                                kernel_G.Evecs.cast<bcg_scalar_t>();
            MatrixS<-1, -1> ID = MatrixS<-1, -1>::Identity(kernel_G.Evals.size(), kernel_G.Evals.size());
            Sigma = (kernel_G.Evecs.cast<bcg_scalar_t>()
                     * kernel_G.Evals.cast<bcg_scalar_t>().asDiagonal()
                     * (ID - S * (ID * (lambda * sigma_squared /
                                        kernel_G.Evals.cast<bcg_scalar_t>().array()).matrix().asDiagonal() +
                                  S).inverse()) *
                     kernel_G.Evecs.cast<bcg_scalar_t>().transpose()) / lambda;
            break;
        }
        case CoherenceType::__last__ : {
            break;
        }
    }
    Y = (MapConst(source_positions) * source_model->linear().transpose()).rowwise() + t.transpose();
    MatrixS<-1, 3> V =
            s * s / sigma_squared * Sigma * MapConst(P1).asDiagonal() * (transform_inverse(MapConst(PX)) - Y);
    Map(deformation) = V * source_model->linear();
    Map(deformed) = Y + V;

    for (size_t m = 0; m < M; ++m) {
        alpha[m] = std::exp(digamma(kappa + P1[m]) - digamma(kappa * M + N_P));
    }

    bcg_scalar_t sigma_squared_bar = (Sigma.diagonal().asDiagonal() * MapConst(P1)).sum() / N_P;
    VectorS<3> mean_x = MapConst(PX).transpose() * MapConst(P1) / N_P;
    VectorS<3> mean_u = Map(deformed).transpose() * MapConst(P1) / N_P;
    MatrixS<-1, -1> U_hat = Map(deformed).rowwise() - mean_u.transpose();
    MatrixS<-1, -1> S_XU =
            (MapConst(PX).rowwise() - mean_x.transpose()).transpose() * MapConst(P1).asDiagonal() * U_hat / N_P;
    MatrixS<-1, -1> S_UU = U_hat.transpose() * MapConst(P1).asDiagonal() * U_hat / N_P +
                           sigma_squared_bar * MatrixS<-1, -1>::Identity(D, D);
    R = project_on_so(S_XU, true);
    s = (S_XU.transpose() * R).trace() / S_UU.trace();
    t = mean_x - s * R * mean_u;

    Map(deformed) = (s * Map(deformed) * R.transpose()).rowwise() + t.transpose();
    sigma_squared = ((X.transpose() * MapConst(PT1).asDiagonal() * X).trace() -
                     2 * ((MapConst(P1).asDiagonal() * MapConst(PX)).transpose() * Map(deformed)).trace() +
                     (Map(deformed).transpose() * MapConst(P1).asDiagonal() * Map(deformed)).trace()) / (N_P * D) +
                    s * s * sigma_squared_bar;

    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
    *source_model = Translation(t) * Rotation(R) * Scaling(VectorS<3>::Constant(s)) * *source_model;
}


void coherent_point_drift_bayes::update_P_full() {
    kernel_matrix<kernel_precision> kernel_P;
    kernel_P.kernel_type = KernelType::gaussian;
    kernel_P.two_sigma_squared = 2 * sigma_squared;

    Y = transform_source();

    Vector<kernel_precision, -1> weight =
            (-s * s / kernel_P.two_sigma_squared * Sigma.diagonal() * M).array() * MapConst(alpha).array() *
            (1.0 - omega);
    Matrix<kernel_precision, -1, -1> K =
            weight.asDiagonal() * kernel_P.compute_kernel(Y.cast<kernel_precision>(), X.cast<kernel_precision>());
    Vector<kernel_precision, -1> c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega * Map(p_out);
    Map(PT1) = (K * OnesN).cast<bcg_scalar_t>();
    Vector<kernel_precision, -1> denominator = 1.0 / (MapConst(PT1).array().cast<kernel_precision>() + c.array());
    Map(PT1) = (K * denominator).cast<bcg_scalar_t>();
    Map(P1) = (denominator.asDiagonal() * K.transpose() * OnesM).cast<bcg_scalar_t>();
    Map(PX) = (K * denominator.asDiagonal() * X.cast<kernel_precision>()).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

void coherent_point_drift_bayes::update_P_FGT() {
    kernel_matrix<kernel_precision> kernel_P;
    kernel_P.kernel_type = KernelType::gaussian;
    kernel_P.two_sigma_squared = 2 * sigma_squared;

    Y = transform_source();

    Vector<kernel_precision, -1> weight =
            (-s * s / kernel_P.two_sigma_squared * Sigma.diagonal() * M).array() * MapConst(alpha).array() *
            (1.0 - omega);
    Matrix<kernel_precision, -1, -1> K =
            weight.asDiagonal() * kernel_P.compute_kernel(Y.cast<kernel_precision>(), X.cast<kernel_precision>());
    Vector<kernel_precision, -1> c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega * Map(p_out);
    Vector<kernel_precision, -1> a = 1.0 / ((K.transpose() * OnesM) + c).array();
    Map(PT1) = (OnesN.array() - c.array() * a.array()).cast<bcg_scalar_t>();
    Map(P1) = (K * a).cast<bcg_scalar_t>();
    Map(PX) = (K * (X.array().cast<kernel_precision>().colwise() * a.array()).matrix()).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

void coherent_point_drift_bayes::update_P_parallel(size_t parallel_grain_size) {
    VectorS<-1> denominator(N);
    kernel_precision two_sigma_squared = 2 * sigma_squared;
    Vector<kernel_precision, -1> weight =
            (-s * s / two_sigma_squared * Sigma.diagonal() * M).array() * MapConst(alpha).array() *
            (1.0 - omega);
    Vector<kernel_precision, -1> c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega * Map(p_out);

    Y = transform_source();

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
                    denominator[n] = PT1[n] + c[n];
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
                        PX[m] += k_mn * X;
                    }
                    sum = sum + P1[m];
                }
            }
    );
    N_P = sum;
}

void coherent_point_drift_bayes::update_P_nystroem() {
    using kernel_precision = double;
    kernel_matrix<kernel_precision> kernel_P;
    kernel_P.kernel_type = KernelType::gaussian;
    kernel_P.two_sigma_squared = 2 * sigma_squared;
    num_samples = std::min<int>(num_samples, M + N);

    Y = transform_source();

    Vector<kernel_precision, -1> weight =
            (-s * s / kernel_P.two_sigma_squared * Sigma.diagonal() * M).array() * MapConst(alpha).array() *
            (1.0 - omega);
    kernel_P.compute_nyström_approximation(Y.cast<kernel_precision>(), X.cast<kernel_precision>(), num_samples);

    if (debug_output) {
        std::cout << "error: "
                  << kernel_P.approximation_error(Y.cast<kernel_precision>(), X.cast<kernel_precision>())
                  << "\n";
    }
    Vector<kernel_precision, -1> c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega * Map(p_out);

    Map(PT1) = (weight.asDiagonal() * kernel_P.K_AV *
                (kernel_P.K_VV_INV * (kernel_P.K_BV.transpose() * OnesN))).cast<bcg_scalar_t>();
    Vector<kernel_precision, -1> denominator = 1.0 / (MapConst(PT1).array().cast<kernel_precision>() + c.array());
    Map(PT1) = (weight.asDiagonal() * kernel_P.K_AV * (kernel_P.K_VV_INV *
                                                       (kernel_P.K_BV.transpose() * denominator))).cast<bcg_scalar_t>();
    Map(P1) = (denominator.asDiagonal() *
               (kernel_P.K_BV * (kernel_P.K_VV_INV * (kernel_P.K_AV.transpose() * weight)))).cast<bcg_scalar_t>();
    Map(PX) = ((weight.asDiagonal() * kernel_P.K_AV *
                (kernel_P.K_VV_INV * (kernel_P.K_BV.transpose() * denominator.asDiagonal() *
                                      X.cast<kernel_precision>())))).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

void coherent_point_drift_bayes::update_P_nystroem_FGT() {
    using kernel_precision = double;
    kernel_matrix<kernel_precision> kernel_P;
    kernel_P.kernel_type = KernelType::gaussian;
    kernel_P.two_sigma_squared = 2 * sigma_squared;
    num_samples = std::min<int>(num_samples, M + N);

    Y = transform_source();

    Vector<kernel_precision, -1> weight =
            (-s * s / kernel_P.two_sigma_squared * Sigma.diagonal() * M).array() * MapConst(alpha).array();

    kernel_P.compute_nyström_approximation(Y.cast<kernel_precision>(), X.cast<kernel_precision>(), num_samples);

    Vector<kernel_precision, -1> c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega * Map(p_out) / (1.0 - omega);

    if (debug_output) {
        std::cout << "error: " << kernel_P.approximation_error(Y.cast<kernel_precision>(), X.cast<kernel_precision>())
                  << "\n";
    }

    Vector<kernel_precision, -1> a = 1.0 / (kernel_P.K_BV * (kernel_P.K_VV_INV * (kernel_P.K_AV.transpose() * weight)) +
                                            c).array();
    Map(PT1) = (OnesN.array() - c.array() * a.array()).cast<bcg_scalar_t>();
    Map(P1) = (weight.asDiagonal() * kernel_P.K_AV *
               (kernel_P.K_VV_INV * (kernel_P.K_BV.transpose() * a))).cast<bcg_scalar_t>();
    Map(PX) = (weight.asDiagonal() * kernel_P.K_AV * (kernel_P.K_VV_INV * (kernel_P.K_BV.transpose() *
                                                                           (X.array().cast<kernel_precision>().colwise() *
                                                                            a.array()).matrix()))).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

void coherent_point_drift_bayes::update_P_kdtree(size_t parallel_grain_size) {
    Matrix<kernel_precision, -1, -1> K = Matrix<kernel_precision, -1, -1>::Zero(M, N);

    Y = transform_source();

    kernel_precision two_sigma_squared = 2 * sigma_squared;

    Vector<kernel_precision, -1> weight =
            (-s * s / two_sigma_squared * Sigma.diagonal() * M).array() * MapConst(alpha).array();
    Vector<kernel_precision, -1> c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega * Map(p_out) / (1.0 - omega);

    Transform target_model_inverse = target_model->inverse();
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) M, parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t m = range.begin(); m != range.end(); ++m) {
                    VectorS<3> Y_target_space = target_model_inverse * Y.row(m);
                    auto result = target_kdtree.query_radius(Y_target_space, 7 * sigma_squared);
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

    Vector<kernel_precision, -1> a = 1.0 / ((K.transpose() * weight) + c).array();
    Map(PT1) = (OnesN.array() - c.array() * a.array()).cast<bcg_scalar_t>();
    Map(P1) = (weight.asDiagonal() * K * a).cast<bcg_scalar_t>();
    Map(PX) = (weight.asDiagonal() * K *
               (X.array().cast<kernel_precision>().colwise() * a.array()).matrix()).cast<bcg_scalar_t>();
    N_P = Map(P1).sum();
}

MatrixS<-1, 3> coherent_point_drift_bayes::transform_inverse(const MatrixS<-1, 3> &residual) {
    return (residual.rowwise() - t.transpose()) * R / s;
}

MatrixS<-1, 3> coherent_point_drift_bayes::transform_source() {
    return (s * (MapConst(source_positions) + MapConst(deformation)) * R.transpose()).rowwise() + t.transpose();
}

VectorS<3> coherent_point_drift_bayes::transform_source(size_t idx) {
    return s * (source_positions[idx] + deformation[idx]).transpose() * R.transpose() + t.transpose();
}


}