//
// Created by alex on 07.12.20.
//

#include <iostream>
#include "bcg_coherent_point_drift_base.h"
#include "tbb/tbb.h"
#include "utils/bcg_timer.h"

namespace bcg {

std::vector<std::string> names_type_p() {
    std::vector<std::string> names(static_cast<int>(coherent_point_drift_base::TypeP::__last__));
    names[static_cast<int>(coherent_point_drift_base::TypeP::full)] = "full";
    names[static_cast<int>(coherent_point_drift_base::TypeP::parallel)] = "parallel";
    names[static_cast<int>(coherent_point_drift_base::TypeP::nyström)] = "nyström";
    return names;
}

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
    initialized = true;
    N_P = 0;
}

void coherent_point_drift_base::maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {}

void coherent_point_drift_base::optimized_expectation_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X,
                                                           size_t parallel_grain_size) {
    Timer timer;
    kernel_precision c = std::pow(2 * pi * sigma_squared, D / 2.0) * omega / (1.0 - omega) * kernel_precision(M) /
                         kernel_precision(N);


    switch (p_type) {
        case TypeP::full: {
            kernel_P.two_sigma_squared = 2 * sigma_squared;

            Matrix<kernel_precision, -1, -1> K = kernel_P.compute_kernel(Y.cast<kernel_precision>(),
                                                                         X.cast<kernel_precision>());
/*            PT1 = (K * Vector<kernel_precision, -1>::Ones(N)).cast<bcg_scalar_t>();
            K = K * (PT1.array().cast<kernel_precision>() + c).matrix().asDiagonal();
            PT1 = (K * Vector<kernel_precision, -1>::Ones(N)).cast<bcg_scalar_t>();
            P1 = (K.transpose() * Vector<kernel_precision, -1>::Ones(M)).cast<bcg_scalar_t>();
            PX = (K * X.cast<kernel_precision>()).cast<bcg_scalar_t>();
            N_P = P1.sum();*/

            Vector<kernel_precision, -1> a = 1.0 / ((K.transpose() * Vector<kernel_precision, -1>::Ones(M)) +
                                                    c * Vector<kernel_precision, -1>::Ones(N)).array();
            PT1 = (Vector<kernel_precision, -1>::Ones(N) - c * a).cast<bcg_scalar_t>();
            P1 = (K * a).cast<bcg_scalar_t>();
            PX = (K * (X.cast<kernel_precision>().array().colwise() * a.array()).matrix()).cast<bcg_scalar_t>();
            N_P = P1.sum();
            std::cout << "P_full: " << timer.pretty_report() << "\n";
            break;
        }
        case TypeP::parallel: {
            tbb::parallel_for(
                    tbb::blocked_range<uint32_t>(0u, (uint32_t) N, parallel_grain_size),
                    [&](const tbb::blocked_range <uint32_t> &range) {
                        for (uint32_t n = range.begin(); n != range.end(); ++n) {
                            PT1(n) = 0;
                            for (long m = 0; m < M; ++m) {
                                bcg_scalar_t k_mn = std::exp(
                                        -(X.row(n).transpose() - transformed(Y, m)).squaredNorm() /
                                        (2 * sigma_squared));
                                PT1(n) += k_mn;
                            }
                            denominator(n) = PT1(n) + c;
                            PT1(n) /= denominator(n);
                        }
                    }
            );

            tbb::atomic <bcg_scalar_t> sum = 0;
            tbb::parallel_for(
                    tbb::blocked_range<uint32_t>(0u, (uint32_t) M, parallel_grain_size),
                    [&](const tbb::blocked_range <uint32_t> &range) {
                        for (uint32_t m = range.begin(); m != range.end(); ++m) {
                            P1(m) = 0;
                            PX.row(m) = VectorS<-1>::Zero(D);
                            for (long n = 0; n < N; ++n) {
                                bcg_scalar_t k_mn = std::exp(
                                        -(X.row(n).transpose() - transformed(Y, m)).squaredNorm() /
                                        (2 * sigma_squared)) /
                                                    denominator(n);
                                P1(m) += k_mn;
                                PX.row(m) += k_mn * X.row(n);
                            }
                            sum = sum + P1(m);
                        }
                    }
            );
            N_P = sum;
            std::cout << "P_parallel: " << timer.pretty_report() << "\n";
            break;
        }
        case TypeP::nyström: {
            kernel_P.two_sigma_squared = 2 * sigma_squared;

            num_samples = std::min<int>(num_samples, Y.rows() + X.rows());

            kernel_P.compute_nyström_approximation(Y.cast<kernel_precision>(), X.cast<kernel_precision>(), num_samples);

            std::cout << "error: " << kernel_P.approximation_error(Y.cast<kernel_precision>(), X.cast<kernel_precision>())
                      << "\n";
            Vector<kernel_precision, -1> a = 1.0 / (kernel_P.K_BV * (kernel_P.K_VV_INV * (kernel_P.K_AV.transpose() *
                                                                                          Vector<kernel_precision, -1>::Ones(
                                                                                              M))) +
                                                    c * Vector<kernel_precision, -1>::Ones(N)).array();
            PT1 = (Vector<kernel_precision, -1>::Ones(N) - c * a).cast<bcg_scalar_t>();
            P1 = (kernel_P.K_AV * (kernel_P.K_VV_INV * (kernel_P.K_BV.transpose() * a))).cast<bcg_scalar_t>();
            PX = (kernel_P.K_AV * (kernel_P.K_VV_INV * (kernel_P.K_BV.transpose() *
                                                        (X.cast<kernel_precision>().array().colwise() *
                                                    a.array()).matrix()))).cast<bcg_scalar_t>();
            N_P = P1.sum();
            std::cout << "P_nyström: " << timer.pretty_report() << "\n";
            break;
        }
        case TypeP::__last__: {
            break;
        }
    }
    std::cout << "P1: " << P1.transpose() << std::endl;
    std::cout << "likelihood: " << P1.prod() << std::endl;
}

void coherent_point_drift_base::optimized_maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {}

VectorS<-1> coherent_point_drift_base::transformed(const MatrixS<-1, -1> &Y, long idx) {
    return Y.row(idx);
}

MatrixS<-1, -1> coherent_point_drift_base::transformed(const MatrixS<-1, -1> &Y) {
    return Y;
}

void coherent_point_drift_base::operator()(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X,
                                           size_t parallel_grain_size) {
    optimized_expectation_step(Y, X, parallel_grain_size);
    if (optimized) {
        optimized_maximization_step(Y, X);
    } else {
        maximization_step(Y, X);
    }
}

}