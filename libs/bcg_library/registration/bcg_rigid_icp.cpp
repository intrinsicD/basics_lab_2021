//
// Created by alex on 04.12.20.
//

#include "bcg_rigid_icp.h"
#include "math/rotations/bcg_rotation_matrix_exponential.h"
#include "tbb/tbb.h"

namespace bcg {

Transform
minimize_point_2_point(const MatrixS<-1, 3> &source, const Transform &source_model, const MatrixS<-1, 3> &target,
                       const Transform &target_model, const VectorS<-1> &weights, size_t parallel_grain_size){
    size_t N = source.rows();
    MatrixS<-1, 3> S;
    MatrixS<-1, 3> T;
    if (N > 10 * parallel_grain_size) {
        S.resize(N, 3);
        T.resize(N, 3);
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) N, parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        S.row(i) = source_model * source.row(i).transpose();
                        T.row(i) = target_model * target.row(i).transpose();
                    }
                }
        );
    } else {
        S = ((source_model.linear() * source.transpose()).colwise() + source_model.translation()).transpose();
        T = ((target_model.linear() * target.transpose()).colwise() + target_model.translation()).transpose();
    }

    VectorS<3> source_mean = weighted_mean(S, weights);
    VectorS<3> target_mean = weighted_mean(T, weights);
    Rotation R(minimize_point_2_point < -1, 3 > (S, T, source_mean, target_mean, weights));
    return R * Translation(get_translation(R.matrix(), source_mean, target_mean));
}

Transform
minimize_point_2_point(const MatrixS<-1, 3> &source, const Transform &source_model, const MatrixS<-1, 3> &target,
                       const Transform &target_model, size_t parallel_grain_size) {
    size_t N = source.rows();
    MatrixS<-1, 3> S;
    MatrixS<-1, 3> T;
    if (N > 10 * parallel_grain_size) {
        S.resize(N, 3);
        T.resize(N, 3);
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) N, parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        S.row(i) = source_model * source.row(i).transpose();
                        T.row(i) = target_model * target.row(i).transpose();
                    }
                }
        );
    } else {
        S = ((source_model.linear() * source.transpose()).colwise() + source_model.translation()).transpose();
        T = ((target_model.linear() * target.transpose()).colwise() + target_model.translation()).transpose();
    }
    VectorS<3> source_mean = S.colwise().mean();
    VectorS<3> target_mean = T.colwise().mean();
    Rotation R(minimize_point_2_point < -1, 3 > (S, T, source_mean, target_mean));
    return R * Translation(get_translation(R.matrix(), source_mean, target_mean));
}


Transform minimize_point_2_plane(const MatrixS<-1, 3> &source, const MatrixS<-1, 3> &target,
                                 const MatrixS<-1, 3> &target_normals) {
    MatrixS<6, 6> A(MatrixS<6, 6>::Zero());

    VectorS<6> b(VectorS<6>::Zero());

    for (int i = 0; i < source.rows(); ++i) {
        const VectorS<3> &src = source.row(i);
        const VectorS<3> &dst = target.row(i);
        const VectorS<3> &n = target_normals.row(i);

        VectorS<6> v;
        v.block(0, 0, 3, 1) = src.cross(n);
        v.block(3, 0, 3, 1) = n;

        A += v * v.transpose();

        b += v * (dst - src).dot(n);
    }

    // solve A * x = b
    //This is levenberg marquard
    bcg_scalar_t lambda = 0.1; //lambda = 0.1 is small enough
    A += A.diagonal().asDiagonal() * lambda;

    VectorS<6> wt = A.fullPivHouseholderQr().solve(b);
    return Rotation(matrix_exponential(VectorS<3>(wt[0], wt[1], wt[2]))) * Translation(VectorS<3>(wt[3], wt[4], wt[5]));
}

Transform minimize_point_2_plane(const MatrixS<-1, 3> &source, const MatrixS<-1, 3> &target, const MatrixS<-1, 3> &target_normals, const VectorS<-1> &weights){
    MatrixS<6, 6> A(MatrixS<6, 6>::Zero());

    VectorS<6> b(VectorS<6>::Zero());

    for (int i = 0; i < source.rows(); ++i) {
        const VectorS<3> &src = source.row(i);
        const VectorS<3> &dst = target.row(i);
        const VectorS<3> &n = target_normals.row(i);

        VectorS<6> v;
        v.block(0, 0, 3, 1) = src.cross(n);
        v.block(3, 0, 3, 1) = n;

        A += weights(i) * v * v.transpose();

        b += weights(i) * v * (dst - src).dot(n);
    }

    // solve A * x = b
    //This is levenberg marquard
    bcg_scalar_t lambda = 0.1; //lambda = 0.1 is small enough
    A += A.diagonal().asDiagonal() * lambda;

    VectorS<6> wt = A.fullPivHouseholderQr().solve(b);
    return Rotation(matrix_exponential(VectorS<3>(wt[0], wt[1], wt[2]))) * Translation(VectorS<3>(wt[3], wt[4], wt[5]));
}


Transform minimize_point_2_plane(const MatrixS<-1, 3> &source, const Transform &source_model, const MatrixS<-1, 3> &target, const MatrixS<-1, 3> &target_normals, const Transform &target_model, const VectorS<-1> &weights, size_t parallel_grain_size){
    size_t N = source.rows();
    MatrixS<-1, 3> S;
    MatrixS<-1, 3> T;
    MatrixS<-1, 3> Normals;
    if (N > 10 * parallel_grain_size) {
        S.resize(N, 3);
        T.resize(N, 3);
        Normals.resize(N, 3);
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) N, parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        S.row(i) = source_model * source.row(i).transpose();
                        T.row(i) = target_model * target.row(i).transpose();
                        Normals.row(i) = target_model.linear() * target_normals.row(i).transpose();
                    }
                }
        );
    } else {
        S = ((source_model.linear() * source.transpose()).colwise() + source_model.translation()).transpose();
        T = ((target_model.linear() * target.transpose()).colwise() + target_model.translation()).transpose();
        Normals = (target_model.linear() * target_normals.transpose()).transpose();
    }
    return minimize_point_2_plane(S, T, Normals, weights);
}


Transform
minimize_point_2_plane(const MatrixS<-1, 3> &source, const Transform &source_model, const MatrixS<-1, 3> &target,
                       const MatrixS<-1, 3> &target_normals, const Transform &target_model,
                       size_t parallel_grain_size) {
    size_t N = source.rows();
    MatrixS<-1, 3> S;
    MatrixS<-1, 3> T;
    MatrixS<-1, 3> Normals;
    if (N > 10 * parallel_grain_size) {
        S.resize(N, 3);
        T.resize(N, 3);
        Normals.resize(N, 3);
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) N, parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        S.row(i) = source_model * source.row(i).transpose();
                        T.row(i) = target_model * target.row(i).transpose();
                        Normals.row(i) = target_model.linear() * target_normals.row(i).transpose();
                    }
                }
        );
    } else {
        S = ((source_model.linear() * source.transpose()).colwise() + source_model.translation()).transpose();
        T = ((target_model.linear() * target.transpose()).colwise() + target_model.translation()).transpose();
        Normals = (target_model.linear() * target_normals.transpose()).transpose();
    }
    return minimize_point_2_plane(S, T, Normals);
}


}