//
// Created by alex on 05.12.20.
//

#include "bcg_coherent_point_drift_rigid.h"
#include "math/rotations/bcg_rotation_project_on_so.h"
#include "tbb/tbb.h"

namespace bcg {

void coherent_point_drift_rigid::init(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X, bcg_scalar_t omega) {
    assert(Y.cols() == X.cols());
    assert(omega <= 1 && omega >= 0);
    M = Y.rows();
    N = X.rows();
    D = Y.cols();

    s = 1.0;
    R = MatrixS<-1, -1>::Identity(D, D);
    t = VectorS<-1>::Zero(D);

    sigma_squared = 0;
    for (long i = 0; i < M; ++i) {
        for (long j = 0; j < N; ++j) {
            sigma_squared += (X.row(j) - Y.row(i)).squaredNorm();
        }
    }
    sigma_squared /= bcg_scalar_t(D * M * N);
    P = MatrixS<-1, -1>::Zero(M, N);
}

void coherent_point_drift_rigid::expectation_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    for (long i = 0; i < M; ++i) {
        for (long j = 0; j < N; ++j) {
            P(i, j) = std::exp(-(X.row(j) - (s * R * Y.row(i) + t)).squaredNorm() / (2 * sigma_squared));
        }
    }
    VectorS<-1> denominator = 1.0 / (VectorS<-1>::Ones(M).transpose() * P).array() +
                              std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * bcg_scalar_t(M) /
                              bcg_scalar_t(N);
    P.rowwise() = P * denominator.asDiagonal();
}

void coherent_point_drift_rigid::maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    P1 = P * VectorS<-1>::Ones(N);
    PT1 = VectorS<-1>::Ones(M).transpose() * P;
    N_P = P1.sum();
    mean_x = X.transpose() * PT1 / N_P;
    mean_y = Y.transpose() * P1 / N_P;
    MatrixS<-1, -1> Xhat = X.rowwise() - mean_x.transpose();
    MatrixS<-1, -1> Yhat = Y.rowwise() - mean_y.transpose();
    MatrixS<-1, -1> A((P * Xhat).transpose() * Yhat);
    R = project_on_so(A, true);
    bcg_scalar_t ARtrace = (A.transpose() * R).trace();
    s = ARtrace / (Yhat.transpose() * P1.asDiagonal() * Yhat).trace();
    t = mean_x - s * R * mean_y;
    sigma_squared = ((Xhat.transpose() * PT1.asDiagonal() * Xhat).trace() - s * ARtrace) / (N_P * D);
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
}

void coherent_point_drift_rigid::operator()(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    expectation_step(Y, X);
    maximization_step(Y, X);
}

void coherent_point_drift_rigid::fast(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X, size_t parallel_grain_size) {
    bcg_scalar_t c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * bcg_scalar_t(M) /
                     bcg_scalar_t(N);

    VectorS<-1> denominator(N);
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) N, parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    PT1(i) = 0;
                    for (long j = 0; j < M; ++j) {
                        bcg_scalar_t k_xy = std::exp(-(X.row(i) - (s * R * Y.row(j) + t)).squaredNorm() / (2 * sigma_squared));
                        PT1(i) += k_xy;
                    }
                    denominator(i) = PT1(i) + c;
                    PT1(i) /= denominator(i);
                }
            }
    );

    MatrixS<-1, -1> PX(MatrixS<-1, -1>::Zero(M, D));
    N_P = 0;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) M, parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    P1(i) = 0;
                    for (long j = 0; j < N; ++j) {
                        bcg_scalar_t k_xy = std::exp(-(X.row(j) - (s * R * Y.row(i) + t)).squaredNorm() / (2 * sigma_squared)) / denominator(j);
                        P1(i) += k_xy;
                        PX.row(i) += k_xy * X.row(j);
                    }
                    N_P += P1(i);
                }
            }
    );


    mean_x = X.transpose() * PT1 / N_P;
    mean_y = Y.transpose() * P1 / N_P;
    MatrixS<-1, -1> A((PX).transpose() * Y - N_P * mean_x * mean_y.transpose());
    R = project_on_so(A, true);
    bcg_scalar_t ARtrace = (A.transpose() * R).trace();
    s = ARtrace / ((P1.array() * Y.rowwise().squaredNorm().array()).sum() - N_P * mean_y.squaredNorm());
    t = mean_x - s * R * mean_y;
    sigma_squared = ((PT1.array() * X.rowwise().squaredNorm().array()).sum() - N_P * mean_x.squaredNorm() - s * ARtrace) / (N_P * D);
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
}

}