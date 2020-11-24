//
// Created by alex on 24.11.20.
//

#ifndef BCG_GRAPHICS_BCG_PCA_H
#define BCG_GRAPHICS_BCG_PCA_H

#include "math/matrix/bcg_matrix_covariance.h"
#include "Eigen/Eigenvalues"

namespace bcg{

template<int N>
struct pca{
    MatrixS<N, N> directions;
    VectorS<N> loadings;
    VectorS<N> mean;
};

template<int N, int D>
inline void LeastSquaresFitSVD(pca<D> &pca, const MatrixS<N, D> &P, const VectorS<D> &mean){
    Eigen::JacobiSVD<MatrixS<N, D>> svd((P.rowwise() - mean.transpose()) / std::sqrt(P.rows() - 1), Eigen::ComputeFullV);
    pca.directions = svd.matrixV();
    pca.loadings = svd.singularValues();
    pca.mean = mean;
}

template<int N, int D>
inline void WeightedLeastSquaresFitSVD(pca<D> &pca, const MatrixS<N, D> &P, const VectorS<D> &mean, const VectorS<N> &weights){
    Eigen::JacobiSVD<MatrixS<N, D>> svd((P.rowwise() - mean.transpose()).colwise().array() * weights / std::sqrt(P.rows() - 1), Eigen::ComputeFullV);
    pca.directions = svd.matrixV();
    pca.loadings = svd.singularValues();
    pca.mean = mean;
}

template<int N, int D>
inline void LeastSquaresFitEigen(pca<D> &pca, const MatrixS<N, D> &P, const VectorS<D> &mean){
    MatrixS<N, D> P_hat = P.rowwise() - mean.transpose();
    Eigen::SelfAdjointEigenSolver<MatrixS<D, D>> eig(covariance(P_hat, P_hat));
    pca.components = eig.eigenvectors().rowwise().reverse();
    pca.loadings = eig.eigenvalues().reverse();
    pca.mean = mean;
}

template<int N, int D>
inline void WeightedLeastSquaresFitEigen(pca<D> &pca, const MatrixS<N, D> &P, const VectorS<D> &mean, const VectorS<N> &weights){
    MatrixS<N, D> P_hat = P.rowwise() - mean.transpose();
    Eigen::SelfAdjointEigenSolver<MatrixS<D, D>> eig(covariance(P_hat, weights, P_hat));
    pca.components = eig.eigenvectors().rowwise().reverse();
    pca.loadings = eig.eigenvalues().reverse();
    pca.mean = mean;
}

}

#endif //BCG_GRAPHICS_BCG_PCA_H
