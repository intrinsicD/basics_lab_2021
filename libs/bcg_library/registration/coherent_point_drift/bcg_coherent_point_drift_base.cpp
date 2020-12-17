//
// Created by alex on 07.12.20.
//

#include <iostream>
#include "bcg_coherent_point_drift_base.h"
#include "tbb/tbb.h"
#include "utils/bcg_timer.h"

namespace bcg {

void coherent_point_drift_base::init(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    assert(Y.cols() == X.cols());
    assert(omega <= 1 && omega >= 0);
    M = Y.rows();
    N = X.rows();
    D = Y.cols();
    PX = MatrixS<-1, -1>::Zero(M, D);
    denominator = PT1 = VectorS<-1>::Zero(N);
    P1 = VectorS<-1>::Zero(M);
    sigma_squared = 0;
    for (long i = 0; i < M; ++i) {
        for (long j = 0; j < N; ++j) {
            sigma_squared += (X.row(j) - Y.row(i)).squaredNorm();
        }
    }

    sigma_squared /= bcg_scalar_t(D * M * N);
}

const MatrixS<-1, -1> &
coherent_point_drift_base::expectation_step(MatrixS<-1, -1> &P, const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    MatrixS<-1, -1> T = transformed(Y);
    P = (-(VectorS<-1>::Ones(M) * X.rowwise().squaredNorm().transpose()
           - (2 * T) * X.transpose() +
           T.rowwise().squaredNorm() * VectorS<-1>::Ones(N).transpose())  / (2 * sigma_squared)).array().exp();

    denominator = 1.0 / ((VectorS<-1>::Ones(M).transpose() * P).array() +
                              std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * bcg_scalar_t(M) /
                              bcg_scalar_t(N));
    P = P * denominator.asDiagonal();
    PX = P * X;
    P1 = P * VectorS<-1>::Ones(N);
    PT1 = VectorS<-1>::Ones(M).transpose() * P;
    N_P = P1.sum();

    optimized_expectation_step(Y, X, 1024);
    return P;
}

void coherent_point_drift_base::maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {}

void coherent_point_drift_base::optimized_expectation_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X,
                                                           size_t parallel_grain_size) {
    bcg_scalar_t c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * bcg_scalar_t(M) /
                     bcg_scalar_t(N);

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) N, parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t n = range.begin(); n != range.end(); ++n) {
                    PT1(n) = 0;
                    for (long m = 0; m < M; ++m) {
                        bcg_scalar_t k_mn = std::exp(
                                -(X.row(n).transpose() - transformed(Y, m)).squaredNorm() / (2 * sigma_squared));
                        PT1(n) += k_mn;
                    }
                    denominator(n) = PT1(n) + c;
                    PT1(n) /= denominator(n);
                }
            }
    );

    tbb::atomic<bcg_scalar_t> sum = 0;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) M, parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t m = range.begin(); m != range.end(); ++m) {
                    P1(m) = 0;
                    PX.row(m) = VectorS<-1>::Zero(D);
                    for (long n = 0; n < N; ++n) {
                        bcg_scalar_t k_mn = std::exp(
                                -(X.row(n).transpose() - (transformed(Y, m))).squaredNorm() / (2 * sigma_squared)) /
                                            denominator(n);
                        P1(m) += k_mn;
                        PX.row(m) += k_mn * X.row(n);
                    }
                    sum = sum + P1(m);
                }
            }
    );
    N_P = sum;
}

void coherent_point_drift_base::optimized_maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {}

VectorS<-1> coherent_point_drift_base::transformed(const MatrixS<-1, -1> &Y, long idx) {
    return Y.row(idx);
}

MatrixS<-1, -1> coherent_point_drift_base::transformed(const MatrixS<-1, -1> &Y){
    return Y;
}

void coherent_point_drift_base::operator()(MatrixS<-1, -1> &P, const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X,
                                           size_t parallel_grain_size) {

    if (optimized) {
        optimized_expectation_step(Y, X, parallel_grain_size);
        optimized_maximization_step(Y, X);
    } else {
        expectation_step(P, Y, X);
        maximization_step(Y, X);
    }
}

}