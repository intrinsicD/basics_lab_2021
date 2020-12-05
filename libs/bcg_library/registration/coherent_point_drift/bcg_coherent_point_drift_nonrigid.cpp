//
// Created by alex on 05.12.20.
//

#include "bcg_coherent_point_drift_nonrigid.h"

namespace bcg {

void coherent_point_drift_nonrigid::init(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X, bcg_scalar_t omega) {
    assert(Y.cols() == X.cols());
    assert(omega <= 1 && omega >= 0);
    assert(beta > 0);
    assert(lambda > 0);
    M = Y.rows();
    N = X.rows();
    D = Y.cols();

    sigma_squared = 0;
    for (long i = 0; i < M; ++i) {
        for (long j = 0; j < N; ++j) {
            sigma_squared += (X.row(j) - Y.row(i)).squaredNorm();
        }
    }
    sigma_squared /= bcg_scalar_t(D * M * N);
    P = MatrixS<-1, -1>::Zero(M, N);
    G = MatrixS<-1, -1>::Zero(M, M);
    for (long i = 0; i < M; ++i) {
        for (long j = 0; j < M; ++j) {
            G(i, j) = std::exp(-(Y.row(j) - Y.row(i)).squaredNorm() / (2 * beta * beta));
        }
    }
    T = Y;
}

void coherent_point_drift_nonrigid::expectation_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    for (long i = 0; i < M; ++i) {
        for (long j = 0; j < N; ++j) {
            P(i, j) = std::exp(-(X.row(j) - T.row(i)).squaredNorm() / (2 * sigma_squared));
        }
    }
    VectorS<-1> denominator = 1.0 / (VectorS<-1>::Ones(M).transpose() * P).array() +
                              std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * bcg_scalar_t(M) /
                              bcg_scalar_t(N);
    P = P * denominator.asDiagonal();
}

void coherent_point_drift_nonrigid::maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    P1 = P * VectorS<-1>::Ones(N);
    PT1 = VectorS<-1>::Ones(M).transpose() * P;
    N_P = P1.sum();
    MatrixS<-1, -1> A = (P1.asDiagonal() * G + lambda * sigma_squared * MatrixS<-1, -1>::Identity(M, M));
    MatrixS<-1, -1> W = A.colPivHouseholderQr().solve(P * X - P1.asDiagonal() * Y);
    T = Y + G * W;

    sigma_squared = ((X.transpose() * PT1.asDiagonal() * X).trace() - 2.0 * ((P * X).transpose() * T).trace() +
                     (T.transpose() * P1.asDiagonal() * T).trace()) / (N_P * D);
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
}

void coherent_point_drift_nonrigid::operator()(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    expectation_step(Y, X);
    maximization_step(Y, X);
}

}