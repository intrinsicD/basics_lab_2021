//
// Created by alex on 05.12.20.
//

#include <iostream>
#include "bcg_coherent_point_drift_nonrigid.h"
#include "utils/bcg_timer.h"
#include "Spectra/SymEigsSolver.h"

namespace bcg {

void coherent_point_drift_nonrigid::init(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    coherent_point_drift_base::init(Y, X);
    assert(beta > 0);
    assert(lambda > 0);

    if (!optimized) {
        P = MatrixS<-1, -1>::Zero(M, N);
    }

    G = MatrixS<-1, -1>::Zero(M, M);
    G = (-(VectorS<-1>::Ones(M) * Y.rowwise().squaredNorm().transpose()
           - (2 * Y) * Y.transpose() +
           Y.rowwise().squaredNorm() * VectorS<-1>::Ones(M).transpose()) / (2 * beta * beta)).array().exp();

    int nev = std::min<int>(M, 100);
    Spectra::DenseSymMatProd<bcg_scalar_t> op(G);
    Spectra::SymEigsSolver<bcg_scalar_t, /*Spectra::LARGEST_ALGE*/Spectra::LARGEST_MAGN, Spectra::DenseSymMatProd<bcg_scalar_t> > eigs(&op, nev,
                                                                                                  2 * nev);
    eigs.init();
    int nconv = eigs.compute();
    if (eigs.info() == Spectra::SUCCESSFUL){
        Evals = eigs.eigenvalues();
        Evecs = eigs.eigenvectors();
        std::cout << "#Eigenvalues converged:" << nconv << std::endl;
        std::cout << "Max Eval :" << Evals[0] << std::endl;
        std::cout << "Min Eval :" << Evals[99] << std::endl;
        std::cout << "Prop. #Eval :" << std::cbrt(Evals[0]) << std::endl;
        std::cout << Evecs.transpose() * Evecs << std::endl;
    }

    T = Y;
}

void coherent_point_drift_nonrigid::maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    MatrixS<-1, -1> A_inv = (P1.array() / (lambda * sigma_squared)).matrix().asDiagonal();
    MatrixS<-1, -1> C_inv = (1.0 / Evals.array()).matrix().asDiagonal();
    // TODO maybo evaluate inner inverse first, seems to be faster...
    MatrixS<-1, -1> inv_inner = (C_inv + Evecs.transpose() * A_inv * Evecs).inverse();
    MatrixS<-1, -1> rhs = A_inv * ((1.0 / P1.array()).matrix().asDiagonal() * PX - Y);
    MatrixS<-1, -1> W = rhs - A_inv * Evecs * inv_inner * Evecs.transpose() * rhs;
    //T = Y + G * W;
    T = Y + (Evecs * (Evals.asDiagonal() * (Evecs.transpose() * W)));


/*    MatrixS<-1, -1> A = (P1.asDiagonal() * G + lambda * sigma_squared * MatrixS<-1, -1>::Identity(M, M));
    bcg_scalar_t lambda = 0.1; //lambda = 0.1 is small enough
    A += A.diagonal().asDiagonal() * lambda;
    MatrixS<-1, -1> W = A.colPivHouseholderQr().solve(PX - P1.asDiagonal() * Y);
    T = Y + G * W;*/

    sigma_squared = ((X.transpose() * PT1.asDiagonal() * X).trace() - 2.0 * ((PX).transpose() * T).trace() +
                     (T.transpose() * P1.asDiagonal() * T).trace()) / (N_P * D);
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
}

void coherent_point_drift_nonrigid::optimized_maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    maximization_step(Y, X);
}

VectorS<-1> coherent_point_drift_nonrigid::transformed(const MatrixS<-1, -1> &Y, long idx) {
    return T.row(idx);
}

MatrixS<-1, -1> coherent_point_drift_nonrigid::transformed(const MatrixS<-1, -1> &Y) {
    return T;
}


}