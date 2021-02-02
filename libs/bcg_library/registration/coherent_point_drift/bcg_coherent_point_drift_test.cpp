//
// Created by alex on 10.12.20.
//

#include <iostream>
#include "bcg_coherent_point_drift_test.h"
#include "Spectra/SymEigsSolver.h"
#include "math/rotations/bcg_rotation_project_on_so.h"

namespace bcg {

void coherent_point_drift_test::init(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    coherent_point_drift_base::init(Y, X);
    assert(beta > 0);
    assert(lambda > 0);

    R = MatrixS<-1, -1>::Identity(D, D);
    t = VectorS<-1>::Zero(D);
    s = 1.0;
    G = (-(VectorS<-1>::Ones(M) * Y.rowwise().squaredNorm().transpose()
           - (2 * Y) * Y.transpose() +
           Y.rowwise().squaredNorm() * VectorS<-1>::Ones(M).transpose()) / (2 * beta * beta)).array().exp();

    int nev = std::min<int>(M, M / 2);
    Spectra::DenseSymMatProd<bcg_scalar_t> op(G);
    Spectra::SymEigsSolver<bcg_scalar_t, /*Spectra::LARGEST_ALGE*/Spectra::LARGEST_MAGN, Spectra::DenseSymMatProd<bcg_scalar_t> > eigs(
            &op, nev,
            M);
    eigs.init();
    int nconv = eigs.compute();
    if (eigs.info() == Spectra::SUCCESSFUL) {
        Evals = eigs.eigenvalues();
        Evecs = eigs.eigenvectors();
        std::cout << "#Eigenvalues converged:" << nconv << std::endl;
    }
    V = MatrixS<-1, -1>::Zero(M, D);
    U = Y;
    T = Y;
}

void coherent_point_drift_test::maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    mean_x = X.transpose() * PT1 / N_P;
    mean_y = Y.transpose() * P1 / N_P;
    MatrixS<-1, -1> A(PX.transpose() * Y - N_P * mean_x * mean_y.transpose());
    R = project_on_so(A, true);
    bcg_scalar_t ARtrace = (A.transpose() * R).trace();
    s = ARtrace / ((P1.array() * Y.rowwise().squaredNorm().array()).sum() - N_P * mean_y.squaredNorm());
    t = mean_x - s * R * mean_y;

    MatrixS<-1, -1> A_inv = (P1.array() / (lambda * sigma_squared)).matrix().asDiagonal();
    MatrixS<-1, -1> C_inv = (1.0 / Evals.array()).matrix().asDiagonal();
    // TODO maybo evaluate inner inverse first, seems to be faster...
    MatrixS<-1, -1> inv_inner = (C_inv + Evecs.transpose() * A_inv * Evecs).inverse();
    VectorS<-1> diagonal = 1.0 / P1.array();
    MatrixS<-1, -1> residual = transformed_inverse(diagonal.asDiagonal() * PX);
    MatrixS<-1, -1> rhs = A_inv * (residual - Y);
    MatrixS<-1, -1> W = rhs - A_inv * Evecs * inv_inner * Evecs.transpose() * rhs;
    //T = Y + G * W;
    V = (Evecs * (Evals.asDiagonal() * (Evecs.transpose() * W)));
    //TODO maybe this is not necessary anymore if i use Y in rotation and scale estimation
    VectorS<-1> mean_V = V.colwise().mean();
    V = (V.rowwise() - mean_V.transpose());
    U = Y + V;


/*    MatrixS<-1, -1> A = (P1.asDiagonal() * G + lambda * sigma_squared * MatrixS<-1, -1>::Identity(M, M));
    bcg_scalar_t lambda = 0.1; //lambda = 0.1 is small enough
    A += A.diagonal().asDiagonal() * lambda;
    MatrixS<-1, -1> W = A.colPivHouseholderQr().solve(PX - P1.asDiagonal() * Y);
    T = Y + G * W;*/
    T = (s * U * R.transpose()).rowwise() + t.transpose();
    sigma_squared = ((X.transpose() * PT1.asDiagonal() * X).trace() - 2.0 * ((PX).transpose() * T).trace() +
                     (T.transpose() * P1.asDiagonal() * T).trace()) / (N_P * D);
    if (std::isnan(sigma_squared)) {
        std::cout << "Sigma^2 is NaN!\n";
    }
    if (T.array().hasNaN()) {
        std::cout << "T has NaN!\n";
    }
    if (U.array().hasNaN()) {
        std::cout << "U has NaN!\n";
    }
    if (V.array().hasNaN()) {
        std::cout << "V has NaN!\n";
    }
    if (W.array().hasNaN()) {
        std::cout << "W has NaN!\n";
    }
    if (rhs.array().hasNaN()) {
        std::cout << "rhs has NaN!\n";
    }
    if (diagonal.array().hasNaN()) {
        std::cout << "diagonal has NaN!\n";
    }
    if (residual.array().hasNaN()) {
        std::cout << "residual has NaN!\n";
    }
    if (inv_inner.array().hasNaN()) {
        std::cout << "inv_inner has NaN!\n";
    }
    if (C_inv.array().hasNaN()) {
        std::cout << "C_inv has NaN!\n";
    }
    if (A_inv.array().hasNaN()) {
        std::cout << "A_inv has NaN!\n";
    }
    if (A.array().hasNaN()) {
        std::cout << "A has NaN!\n";
    }
    if (t.array().hasNaN()) {
        std::cout << "t has NaN!\n";
    }
    if (std::isnan(s)) {
        std::cout << "s is NaN!\n";
    }
    if (s == 0) {
        std::cout << "s is Zero!\n";
    }
    if (R.array().hasNaN()) {
        std::cout << "R has NaN!\n";
    }
    if (PT1.array().hasNaN()) {
        std::cout << "PT1 has NaN!\n";
    }
    if (P1.array().hasNaN()) {
        std::cout << "P1 has NaN!\n";
    }
    if (PX.array().hasNaN()) {
        std::cout << "PX has NaN!\n";
    }
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
}


void coherent_point_drift_test::optimized_maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    maximization_step(Y, X);
}

VectorS<-1> coherent_point_drift_test::transformed(const MatrixS<-1, -1> &Y, long idx) {
    return T.row(idx);
}

MatrixS<-1, -1> coherent_point_drift_test::transformed(const MatrixS<-1, -1> &Y) {
    return T;
}

MatrixS<-1, -1> coherent_point_drift_test::transformed_inverse(const MatrixS<-1, -1> &Y) {
    return (Y.rowwise() - t.transpose()) / s * R;
}

}