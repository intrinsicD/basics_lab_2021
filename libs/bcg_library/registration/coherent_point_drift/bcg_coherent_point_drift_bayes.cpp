//
// Created by alex on 05.12.20.
//

#include "bcg_coherent_point_drift_bayes.h"
#include "math/rotations/bcg_rotation_project_on_so.h"
#include "tbb/tbb.h"

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
    coherent_point_drift_base::init(Y, X, omega);
    assert(beta > 0);
    assert(lambda > 0);
    assert(gamma > 0);
    sigma_squared *= gamma;
    s = 1.0;
    R = MatrixS<-1, -1>::Identity(D, D);
    t = VectorS<-1>::Zero(D);
    alpha = VectorS<-1>::Constant(M, 1.0 / M);
    p_out = VectorS<-1>::Constant(N, 1.0 / N);
    if (!optimized) {
        P = MatrixS<-1, -1>::Zero(M, N);
    }
    GInv = MatrixS<-1, -1>::Zero(M, M);
    MatrixS<-1, -1> G = MatrixS<-1, -1>::Zero(M, M);
    Sigma = MatrixS<-1, -1>::Identity(M, M);

    G = (-(VectorS<-1>::Ones(M) * Y.rowwise().squaredNorm().transpose()
           - (2 * Y) * Y.transpose() +
           Y.rowwise().squaredNorm() * VectorS<-1>::Ones(M).transpose()) / (2 * beta * beta)).array().exp();

    GInv = G.inverse();
    T = Y;
}

const MatrixS<-1, -1> &
coherent_point_drift_bayes::expectation_step(MatrixS<-1, -1> &P, const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    VectorS<-1> weight = (-s * s * Sigma.diagonal().array().square().pow(D) / (2 * sigma_squared)).exp();
    bcg_scalar_t normalizer = std::sqrt((2 * pi * sigma_squared * MatrixS<-1, -1>::Identity(D, D)).determinant());
    for (long i = 0; i < M; ++i) {
        for (long j = 0; j < N; ++j) {
            P(i, j) = std::exp(-(X.row(j).transpose() - transformed(Y, i)).squaredNorm() / (2 * sigma_squared)) * (1 - omega) * alpha(i) *
                      weight(i) / normalizer;
        }
    }
    denominator = 1.0 / ((VectorS<-1>::Ones(M).transpose() * P).array() + omega * p_out.transpose().array());
    P = P * denominator.asDiagonal();
    PX = P * X;
    P1 = P * VectorS<-1>::Ones(N);
    PT1 = VectorS<-1>::Ones(M).transpose() * P;
    N_P = P1.sum();
    return P;
}

void coherent_point_drift_bayes::maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    MatrixS<-1, -1> SigmaInv = lambda * GInv + s * s / sigma_squared * P1.asDiagonal().toDenseMatrix();
    MatrixS<-1, -1> residual = (1.0 / P1.array()).matrix().asDiagonal() * PX;
    MatrixS<-1, -1> V = SigmaInv.colPivHouseholderQr().solve(
            s * s / sigma_squared * P1.asDiagonal() * ((residual.rowwise() - t.transpose()) / s * R - Y));
    U = Y + V;

    if (kappa == -1) {
        //alpha = VectorS<-1>::Ones(M);
        alpha = VectorS<-1>::Constant(M, 1.0 / M);
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
    sigma_squared = ((X.transpose() * P1.asDiagonal() * X).trace() - 2 * (PX.transpose() * T).trace() +
                     (T.transpose() * PT1.asDiagonal() * T).trace()) / (N_P * D) + s * s * sigma_squared_bar;
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
}

void coherent_point_drift_bayes::optimized_expectation_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X, size_t parallel_grain_size) {
    VectorS<-1> weight = (-s * s * Sigma.diagonal().array().square().pow(D) / (2 * sigma_squared)).exp();
    bcg_scalar_t normalizer = std::sqrt((2 * pi * sigma_squared * MatrixS<-1, -1>::Identity(D, D)).determinant());

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) N, parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t n = range.begin(); n!= range.end(); ++n) {
                    PT1(n) = 0;
                    for (long m = 0; m < M; ++m) {
                        bcg_scalar_t k_mn = std::exp(-(X.row(n).transpose() - transformed(Y, m)).squaredNorm() / (2 * sigma_squared)) * (1 - omega) * alpha(m) *
                                            weight(m) / normalizer;
                        PT1(n) += k_mn;
                    }
                    denominator(n) = PT1(n) + omega * p_out(n);
                    PT1(n) /= denominator(n);
                }
            }
    );

    tbb::atomic<size_t> sum = 0;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) M, parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t m = range.begin(); m != range.end(); ++m) {
                    P1(m) = 0;
                    PX.row(m) = VectorS<-1>::Zero(D);
                    for (long n = 0; n < N; ++n) {
                        bcg_scalar_t k_mn = std::exp(-(X.row(n).transpose() - transformed(Y, m)).squaredNorm() / (2 * sigma_squared))  * (1 - omega) * alpha(m) *
                                            weight(m) / (normalizer * denominator(n));
                        P1(m) += k_mn;
                        PX.row(m) += k_mn * X.row(n);
                    }
                    sum += P1(m);
                }
            }
    );
    N_P = sum;
}

void coherent_point_drift_bayes::optimized_maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    return maximization_step(Y, X);
}

VectorS<-1> coherent_point_drift_bayes::transformed(const MatrixS<-1, -1> &Y, long idx) {
    return T.row(idx);
}

MatrixS<-1, -1> coherent_point_drift_bayes::transformed(const MatrixS<-1, -1> &Y){
    return T;
}
}