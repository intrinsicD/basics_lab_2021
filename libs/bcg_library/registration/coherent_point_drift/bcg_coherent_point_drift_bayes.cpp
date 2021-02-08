//
// Created by alex on 05.12.20.
//

#include <iostream>
#include <random>
#include <algorithm>
#include <numeric>
#include "bcg_coherent_point_drift_bayes.h"
#include "math/rotations/bcg_rotation_project_on_so.h"
#include "math/vector/bcg_vector_map_eigen.h"
#include "tbb/tbb.h"
#include "utils/bcg_timer.h"
#include "Spectra/SymEigsSolver.h"

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

void coherent_point_drift_bayes::init(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    Timer timer;
    coherent_point_drift_base::init(Y, X);
    assert(beta > 0);
    assert(lambda > 0);
    assert(gamma > 0);
    sigma_squared *= gamma;

    s = 1.0;
    R = MatrixS<-1, -1>::Identity(D, D);
    t = VectorS<-1>::Zero(D);

    alpha = VectorS<-1>::Constant(M, 1.0 / M);
    p_out = VectorS<-1>::Constant(N, 1.0 / N);

    MatrixS<-1, -1> G = MatrixS<-1, -1>::Zero(M, M);
    Sigma = MatrixS<-1, -1>::Identity(M, M);

    G = (-(VectorS<-1>::Ones(M) * Y.rowwise().squaredNorm().transpose()
           - (2 * Y) * Y.transpose() +
           Y.rowwise().squaredNorm() * VectorS<-1>::Ones(M).transpose()) / (2 * beta * beta)).array().exp();

    int nev = std::min<int>(M, 200);
    int nev2 = std::min<int>(M, 2 * nev);
    std::cout << "Compute " << nev << " eigenvectors. \n";
    Spectra::DenseSymMatProd<bcg_scalar_t> op(G);
    Spectra::SymEigsSolver<bcg_scalar_t, /*Spectra::LARGEST_ALGE*/Spectra::LARGEST_MAGN, Spectra::DenseSymMatProd<bcg_scalar_t> > eigs(
            &op, nev,
            nev2);

    eigs.init();
    int nconv = eigs.compute();
    if (eigs.info() == Spectra::SUCCESSFUL) {
        Evals = eigs.eigenvalues();
        Evecs = eigs.eigenvectors();
        std::cout << "#Eigenvalues converged:" << nconv << std::endl;
    }
    //GInv = MatrixS<-1, -1>::Zero(M, M);
    //GInv = G.inverse();
    T = Y;
    U = Y;
    V = MatrixS<-1, -1>::Zero(M, D);

    K_XV = Matrix<bcg_scalar_t, -1, -1>::Zero(N, J);
    K_YV = Matrix<bcg_scalar_t, -1, -1>::Zero(M, J);
    K_VV = Matrix<bcg_scalar_t, -1, -1>::Zero(J, J);

    std::cout << "Init: " << timer.pretty_report() << "\n";
}

template<typename T>
void check(const T &val, const std::string &name){
    if(val.array().hasNaN()){
        std::cout << name << " has NaN\n";
    }
}

void coherent_point_drift_bayes::maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    Timer timer;
    MatrixS<-1, -1> S = Evecs.transpose() * P1.asDiagonal() * Evecs;
    MatrixS<-1, -1> ID = MatrixS<-1, -1>::Identity(Evals.size(), Evals.size());
    Sigma = (Evecs * Evals.asDiagonal() * (ID - S * (ID * (lambda * sigma_squared / Evals.array()).matrix().asDiagonal() + S).inverse()) * Evecs.transpose()) / lambda;
    //Sigma = (lambda * GInv + s * s / sigma_squared * ID * P1.asDiagonal()).inverse();

    MatrixS<-1, -1> residual = (1.0 / P1.array()).matrix().asDiagonal() * PX;
    V = s * s / sigma_squared * Sigma * P1.asDiagonal() * (transformed_inverse(residual) - Y);

    U = Y + V;

    for (size_t m = 0; m < M; ++m) {
        alpha(m) = std::exp(digamma(kappa + P1(m)) - digamma(kappa * M + N_P));
    }

    bcg_scalar_t sigma_squared_bar = (Sigma.diagonal().asDiagonal() * P1).sum() / N_P;
    mean_x = residual.transpose() * P1 / N_P;
    mean_u = U.transpose() * P1 / N_P;
    MatrixS<-1, -1> U_hat = U.rowwise() - mean_u.transpose();
    MatrixS<-1, -1> S_XU = (residual.rowwise() - mean_x.transpose()).transpose() * P1.asDiagonal() * U_hat / N_P;
    MatrixS<-1, -1> S_UU = U_hat.transpose() * P1.asDiagonal() * U_hat / N_P +
                           sigma_squared_bar * MatrixS<-1, -1>::Identity(D, D);
    R = project_on_so(S_XU, true);
    s = (S_XU.transpose() * R).trace() / S_UU.trace();
    t = mean_x - s * R * mean_u;

    T = (s * U * R.transpose()).rowwise() + t.transpose();
    sigma_squared = ((X.transpose() * PT1.asDiagonal() * X).trace() - 2 * (PX.transpose() * T).trace() +
                     (T.transpose() * P1.asDiagonal() * T).trace()) / (N_P * D) + s * s * sigma_squared_bar;

    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
    std::cout << "M-step: " << timer.pretty_report() << "\n";
}

void coherent_point_drift_bayes::optimized_expectation_step_old(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X,
                                                            size_t parallel_grain_size) {
    Timer timer;
    std::vector<int> sampled_union;   // or reserve space for N elements up front
    std::vector<int> indices_union(M + N);
    std::iota(indices_union.begin(), indices_union.end(), 0);   // or reserve space for N elements up front

    J = std::min<int>(J, M);
    std::sample(indices_union.begin(), indices_union.end(), std::back_inserter(sampled_union),
                J, std::mt19937{std::random_device{}()});

    MatrixS<-1, 3> VV(J, D);

    for(size_t i = 0; i < J; ++i){
        if(sampled_union[i] < M){
            VV.row(i) = T.row(sampled_union[i]);
        }else{
            VV.row(i) = X.row(sampled_union[i] - M);
        }
    }
    std::cout << "sampled union: " << Map(sampled_union).transpose() << std::endl;
    K_VV = (-(VectorS<-1>::Ones(J).cast<bcg_scalar_t>() * VV.cast<bcg_scalar_t>().rowwise().squaredNorm().transpose()
           - (2 * VV.cast<bcg_scalar_t>()) * VV.cast<bcg_scalar_t>().transpose() +
            VV.cast<bcg_scalar_t>().rowwise().squaredNorm() * VectorS<-1>::Ones(J).cast<bcg_scalar_t>().transpose()) / (2 * sigma_squared * sigma_squared)).array().exp();

    K_XV = (-(VectorS<-1>::Ones(N).cast<bcg_scalar_t>() * VV.cast<bcg_scalar_t>().rowwise().squaredNorm().transpose()
           - (2 * X.cast<bcg_scalar_t>()) * VV.cast<bcg_scalar_t>().transpose() +
           X.cast<bcg_scalar_t>().rowwise().squaredNorm() * VectorS<-1>::Ones(J).cast<bcg_scalar_t>().transpose()) / (2 * sigma_squared * sigma_squared)).array().exp();

    K_YV = (-(VectorS<-1>::Ones(M).cast<bcg_scalar_t>() * VV.cast<bcg_scalar_t>().rowwise().squaredNorm().transpose()
           - (2 * T.cast<bcg_scalar_t>()) * VV.cast<bcg_scalar_t>().transpose() +
           T.cast<bcg_scalar_t>().rowwise().squaredNorm() * VectorS<-1>::Ones(J).cast<bcg_scalar_t>().transpose()) / (2 * sigma_squared * sigma_squared)).array().exp();

    Matrix<bcg_scalar_t, -1, -1> K_YX = (-(VectorS<-1>::Ones(M).cast<bcg_scalar_t>() * X.cast<bcg_scalar_t>().rowwise().squaredNorm().transpose()
                              - (2 * T.cast<bcg_scalar_t>()) * X.cast<bcg_scalar_t>().transpose() +
                              T.cast<bcg_scalar_t>().rowwise().squaredNorm() * VectorS<-1>::Ones(N).cast<bcg_scalar_t>().transpose()) / (2 * sigma_squared * sigma_squared)).array().exp();

    std::cout << "K_YV:\n" << K_YV << std::endl;
    std::cout << "K_VV:\n" << K_VV << std::endl;
    std::cout << "K_XV:\n" << K_XV << std::endl;

    Matrix<bcg_scalar_t ,-1, -1> K_VV_Inv = K_VV.inverse();

    std::cout << "K_VV_Inv:\n" << K_VV_Inv << std::endl;
    std::cout << "K_YX:\n" << K_YX << std::endl;
    std::cout << "K_YX_NYSTÖM:\n" << K_YV * K_VV_Inv * K_XV.transpose() << std::endl;
    std::cout << "Norm: " << (K_YX - K_YV * K_VV_Inv * K_XV.transpose()).norm() << std::endl;

    VectorS<-1> c_0_prime = omega / (1-omega) * p_out * std::sqrt((2 * pi * sigma_squared * MatrixS<-1, -1>::Identity(D, D)).determinant());
    VectorS<-1> b = alpha.array() * (-s * s * Sigma.diagonal().array() * D / (2 * sigma_squared)).exp();
    VectorS<-1> q = VectorS<-1>::Ones(N).array() / (K_YX.cast<bcg_scalar_t>().transpose() * b + c_0_prime).array();
    //VectorS<-1> q = VectorS<-1>::Ones(N).array() / (K_XV * (K_VV_Inv * (K_YV.transpose() * b)) + c_0_prime).array();

    std::cout << "c_0_prime: " << c_0_prime.transpose() << std::endl;
    std::cout << "b:         " << b.transpose() << std::endl;
    std::cout << "q:         " << q.transpose() << std::endl;

    PT1 = VectorS<-1>::Ones(N) - c_0_prime.cwiseProduct(q);
    P1 = b.cwiseProduct(K_YX.cast<bcg_scalar_t>() * q);
    //P1 = b.cwiseProduct(K_YV * (K_VV_Inv * (K_XV.transpose() * q)));
    PX = (K_YX.cast<bcg_scalar_t>() * (X.array().colwise() * q.array()).matrix()).array().colwise() * b.array();
    //PX = (K_YV * (K_VV_Inv * (K_XV.transpose() * (X.array().colwise() * q.array()).matrix()))).array().colwise() * b.array();

    N_P = P1.sum();

    std::cout << "PT1: " << PT1.transpose() << std::endl;
    std::cout << "P1:  " << P1.transpose() << std::endl;
    std::cout << "PX:\n" << PX.transpose() << std::endl;
    std::cout << "N_P: " << N_P << std::endl;

    std::cout << "E-step: " << timer.pretty_report() << "\n";

    optimized_expectation_step_old(Y, X, parallel_grain_size);
}


void coherent_point_drift_bayes::optimized_expectation_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X,
                                                            size_t parallel_grain_size) {
    Timer timer;
    //bcg_scalar_t normalizer = std::pow(2 * pi * sigma_squared, D / 2.0);
    bcg_scalar_t normalizer = std::sqrt((2 * pi * sigma_squared * MatrixS<-1, -1>::Identity(D, D)).determinant());

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) N, parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t n = range.begin(); n != range.end(); ++n) {
                    PT1(n) = 0;
                    for (long m = 0; m < M; ++m) {
                        bcg_scalar_t weight = std::exp(-s * s * (Sigma(m, m) * MatrixS<-1,-1>::Identity(D, D)).trace() / (2 * sigma_squared));
                        bcg_scalar_t k_mn =
                                std::exp(-(X.row(n).transpose() - transformed(Y, m)).squaredNorm() /
                                         (2 * sigma_squared)) * (1 - omega) * alpha(m) * weight / normalizer;
                        PT1(n) += k_mn;
                    }
                    denominator(n) = PT1(n) + omega * p_out(n);
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
                    bcg_scalar_t weight = std::exp(-s * s * (Sigma(m, m) * MatrixS<-1,-1>::Identity(D, D)).trace() / (2 * sigma_squared));
                    for (long n = 0; n < N; ++n) {
                        bcg_scalar_t k_mn =
                                std::exp(-(X.row(n).transpose() - transformed(Y, m)).squaredNorm() /
                                         (2 * sigma_squared)) * (1 - omega) * alpha(m) * weight /
                                (normalizer * denominator(n));
                        P1(m) += k_mn;
                        PX.row(m) += k_mn * X.row(n);
                    }
                    sum = sum + P1(m);
                }
            }
    );
    N_P = sum;

    std::cout << "PT1: " << PT1.transpose() << std::endl;
    std::cout << "P1:  " << P1.transpose() << std::endl;
    std::cout << "PX:\n" << PX.transpose() << std::endl;
    std::cout << "N_P: " << N_P << std::endl;

    std::cout << "E-step: " << timer.pretty_report() << "\n";
}

void coherent_point_drift_bayes::optimized_maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    return maximization_step(Y, X);
}

VectorS<-1> coherent_point_drift_bayes::transformed(const MatrixS<-1, -1> &Y, long idx) {
    return T.row(idx);
}

MatrixS<-1, -1> coherent_point_drift_bayes::transformed(const MatrixS<-1, -1> &Y) {
    return T;
}

MatrixS<-1, -1> coherent_point_drift_bayes::transformed_inverse(const MatrixS<-1, -1> &X) {
    return (X.rowwise() - t.transpose()) / s * R;
}
}