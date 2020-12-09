//
// Created by alex on 05.12.20.
//

#include "bcg_coherent_point_drift_affine.h"

namespace bcg {

void coherent_point_drift_affine::init(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X, bcg_scalar_t omega) {
    coherent_point_drift_base::init(Y, X, omega);

    B = MatrixS<-1, -1>::Identity(D, D);
    t = VectorS<-1>::Zero(D);

    if (!optimized) {
        P = MatrixS<-1, -1>::Zero(M, N);
    }
}

const MatrixS<-1, -1> &
coherent_point_drift_affine::expectation_step(MatrixS<-1, -1> &P, const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    return coherent_point_drift_base::expectation_step(P, Y, X);
}

void coherent_point_drift_affine::maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    mean_x = X.transpose() * PT1 / N_P;
    mean_y = Y.transpose() * P1 / N_P;
    MatrixS<-1, -1> A(PX.transpose() * Y - N_P * mean_x * mean_y.transpose());
    B = A * (Y.transpose() * P1.asDiagonal() * Y - N_P * mean_y * mean_y.transpose()).inverse();

    t = mean_x - B * mean_y;
    sigma_squared = ((PT1.array() * X.rowwise().squaredNorm().array()).sum() - N_P * mean_x.squaredNorm() - (A * B.transpose()).trace()) /
                    (N_P * D);
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
}

void coherent_point_drift_affine::optimized_expectation_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X, size_t parallel_grain_size) {
    return coherent_point_drift_base::optimized_expectation_step(Y, X, parallel_grain_size);
}

void coherent_point_drift_affine::optimized_maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    maximization_step(Y, X);
}

VectorS<-1> coherent_point_drift_affine::transformed(const MatrixS<-1, -1> &Y, long idx) {
    return B * Y.row(idx).transpose() + t;
}

}