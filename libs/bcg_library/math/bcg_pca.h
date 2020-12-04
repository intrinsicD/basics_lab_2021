//
// Created by alex on 24.11.20.
//

#ifndef BCG_GRAPHICS_BCG_PCA_H
#define BCG_GRAPHICS_BCG_PCA_H

#include "math/matrix/bcg_matrix_covariance.h"
#include "Eigen/Eigenvalues"
#include "Eigen/SVD"

namespace bcg{

template<int D>
struct Pca{
    MatrixS<D, D> directions;
    VectorS<D> loadings;
    VectorS<D> mean;
};

template<int N, int D>
inline void least_squares_fit_svd(Pca<D> &pca, const MatrixS<N, D> &P, const VectorS<D> &mean){
    Eigen::JacobiSVD<MatrixS<N, D>> svd((P.rowwise() - mean.transpose()) / std::sqrt(P.rows() - 1), Eigen::ComputeFullV | Eigen::ComputeFullU);
    pca.directions = svd.matrixV();

    pca.loadings.head(svd.singularValues().size()) = svd.singularValues();
    pca.mean = mean;
}

template<int N, int D>
inline void weighted_least_squares_fit_svd(Pca<D> &pca, const MatrixS<N, D> &P, const VectorS<D> &mean, const VectorS<N> &weights){
    Eigen::JacobiSVD<MatrixS<N, D>> svd(weights.asDiagonal() * (P.rowwise() - mean.transpose())  / std::sqrt(P.rows() - 1), Eigen::ComputeFullV | Eigen::ComputeFullU);
    pca.directions = svd.matrixV();
    pca.loadings = svd.singularValues();
    pca.mean = mean;
}

template<int N, int D>
inline void least_squares_fit_eig(Pca<D> &pca, const MatrixS<N, D> &P, const VectorS<D> &mean){
    MatrixS<N, D> P_hat = P.rowwise() - mean.transpose();
    Eigen::SelfAdjointEigenSolver<MatrixS<D, D>> eig(covariance<N, D>(P_hat, P_hat));
    pca.directions = eig.eigenvectors().rowwise().reverse();
    pca.loadings = eig.eigenvalues().reverse();
    pca.mean = mean;
}

template<int N, int D>
inline void weighted_least_squares_fit_eig(Pca<D> &pca, const MatrixS<N, D> &P, const VectorS<D> &mean, const VectorS<N> &weights){
    MatrixS<N, D> P_hat = P.rowwise() - mean.transpose();
    Eigen::SelfAdjointEigenSolver<MatrixS<D, D>> eig(covariance<N, D>(P_hat, weights, P_hat));
    pca.directions = eig.eigenvectors().rowwise().reverse();
    pca.loadings = eig.eigenvalues().reverse();
    pca.mean = mean;
}

}

#endif //BCG_GRAPHICS_BCG_PCA_H
