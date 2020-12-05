//
// Created by alex on 05.12.20.
//

#include "bcg_coherent_point_drift_affine.h"

namespace bcg{

void coherent_point_drift_affine::init(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X, bcg_scalar_t omega) {
    assert(Y.cols() == X.cols());
    assert(omega <= 1 && omega >= 0);
    M = Y.rows();
    N = X.rows();
    D = Y.cols();

    B = MatrixS<-1, -1>::Identity(D, D);
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

void coherent_point_drift_affine::expectation_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    for (long i = 0; i < M; ++i) {
        for (long j = 0; j < N; ++j) {
            P(i, j) = std::exp(-(X.row(j) - (B * Y.row(i) + t)).squaredNorm() / (2 * sigma_squared));
        }
    }
    VectorS<-1> denominator = 1.0 / (VectorS<-1>::Ones(M).transpose() * P).array() +
                              std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * bcg_scalar_t(M) /
                              bcg_scalar_t(N);
    P.rowwise() = P * denominator.asDiagonal();
}

void coherent_point_drift_affine::maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    P1 = P * VectorS<-1>::Ones(N);
    PT1 = VectorS<-1>::Ones(M).transpose() * P;
    N_P = P1.sum();
    mean_x = X.transpose() * PT1 / N_P;
    mean_y = Y.transpose() * P1 / N_P;
    MatrixS<-1, -1> Xhat = X.rowwise() - mean_x.transpose();
    MatrixS<-1, -1> Yhat = Y.rowwise() - mean_y.transpose();
    MatrixS<-1, -1> A(Xhat.transpose() * P.transpose() * Yhat);
    B = A * (Yhat.transpose() * P1.asDiagonal() * Yhat).inverse();

    t = mean_x - B * mean_y;
    sigma_squared = ((Xhat.transpose() * PT1.asDiagonal() * Xhat).trace() - (A * B.transpose()).trace()) /
                    (N_P * D);
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
}

void coherent_point_drift_affine::operator()(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X){
    expectation_step(Y, X);
    maximization_step(Y, X);
}

}