//
// Created by alex on 05.12.20.
//

#include "bcg_coherent_point_drift_bayes.h"
#include "math/rotations/bcg_rotation_project_on_so.h"

namespace bcg {

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

void coherent_point_drift_bayes::init(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X, bcg_scalar_t omega) {
    assert(Y.cols() == X.cols());
    assert(omega <= 1 && omega >= 0);
    assert(beta > 0);
    assert(lambda > 0);
    M = Y.rows();
    N = X.rows();
    D = Y.cols();

    s = 1.0;
    R = MatrixS<-1, -1>::Identity(D, D);
    t = VectorS<-1>::Zero(D);
    alpha = VectorS<-1>::Constant(M, 1.0 / M);

    sigma_squared = 0;
    for (long i = 0; i < M; ++i) {
        for (long j = 0; j < N; ++j) {
            sigma_squared += (X.row(j) - Y.row(i)).squaredNorm();
        }
    }
    sigma_squared /= bcg_scalar_t(D * M * N);
    P = MatrixS<-1, -1>::Zero(M, N);
    GInv = MatrixS<-1, -1>::Zero(M, M);
    MatrixS<-1, -1> G = MatrixS<-1, -1>::Zero(M, M);
    Sigma = MatrixS<-1, -1>::Identity(M, M);
    for (long i = 0; i < M; ++i) {
        for (long j = 0; j < M; ++j) {
            G(i, j) = std::exp(-(Y.row(j) - Y.row(i)).squaredNorm() / (2 * beta * beta));
        }
    }
    GInv = G.inverse();
    T = Y;
}

void coherent_point_drift_bayes::expectation_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    VectorS<-1> weight = (-s * s * Sigma.diagonal().array().square().pow(D) / (2 * sigma_squared)).exp();
    bcg_scalar_t normalizer = std::sqrt((2 * pi * sigma_squared * MatrixS<-1, -1>::Identity(D, D)).determinant());
    for (long i = 0; i < M; ++i) {
        for (long j = 0; j < N; ++j) {
            P(i, j) = std::exp(-(X.row(j) - T.row(i)).squaredNorm() / (2 * sigma_squared)) * (1 - omega) * alpha(i) *
                       weight(i) / normalizer;
        }
    }
    VectorS<-1> denominator = 1.0 / ((VectorS<-1>::Ones(M).transpose() * P).array() + omega * p_out.array());
    P.rowwise() = P * denominator.asDiagonal();
}

void coherent_point_drift_bayes::maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    P1 = P * VectorS<-1>::Ones(N);
    PT1 = VectorS<-1>::Ones(M).transpose() * P;
    N_P = P1.sum();
    MatrixS<-1, -1> SigmaInv = lambda * GInv + s * s / sigma_squared * P1.asDiagonal().toDenseMatrix();
    MatrixS<-1, -1> residual = P1.inverse().asDiagonal() * P * X;
    MatrixS<-1, -1> V = SigmaInv.colPivHouseholderQr().solve(
            s * s / sigma_squared * P1.asDiagonal() * ((residual.rowwise() - t.transpose()) / s * R - Y));
    U = Y + V;

    if (kappa == -1) {
        alpha = VectorS<-1>::Ones(M);
    } else {
        for (size_t m = 0; m < M; ++m) {
            alpha(m) = std::exp(digamma(kappa + P1(m)) - digamma(kappa * M + N_P));
        }
    }

    mean_x = residual.transpose() * P1 / N_P;
    mean_u = U.transpose() * P1 / N_P;
    bcg_scalar_t sigma_squared_bar = (Sigma.diagonal().array().square().matrix().asDiagonal() * P1).sum() / N_P;
    MatrixS<-1, -1> Xhat = residual.rowwise() - mean_x.transpose();
    MatrixS<-1, -1> Uhat = U.rowwise() - mean_u.transpose();
    MatrixS<-1, -1> S_XU = Xhat.transpose() * P1.asDiagonal() * Uhat / N_P;
    MatrixS<-1, -1> S_UU =
            Uhat.transpose() * P1.asDiagonal() * Uhat / N_P + sigma_squared_bar * MatrixS<-1, -1>::Identity(D, D);
    R = project_on_so(S_XU, true);
    s = (S_XU.transpose() * R).trace() / S_UU.trace();
    t = mean_x - s * R * mean_u;
    T = (s * U * R.transpose()).rowwise() + t.transpose();
    sigma_squared = ((X.transpose() * P1.asDiagonal() * X).trace() - 2 * ((P * X).transpose() * T).trace() +
                     (T.transpose() * PT1.asDiagonal() * T).trace()) / (N_P * D) + s * s * sigma_squared_bar;
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
}

void coherent_point_drift_bayes::operator()(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    expectation_step(Y, X);
    maximization_step(Y, X);
}
}