//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_ROBUST_PCA_H
#define BCG_GRAPHICS_BCG_ROBUST_PCA_H


#include <iostream>
#include "math/matrix/bcg_matrix.h"

/*
 * https://github.com/dlaptev/RobustPCA/blob/master/RobustPCA.m
 * */

namespace bcg {

template<int M, int N, typename Real>
struct robust_pca {
    struct Result {
        MatrixS<M, N> L, S;
    };

    Result operator()(const MatrixS<M, N> &P) const {
        bcg_scalar_t lambda = 1.0 / std::sqrt(std::max(P.rows(), P.cols()));
        return operator()(P, lambda, 10 * lambda);
    }

    Result operator()(const MatrixS<M, N> &P, bcg_scalar_t lambda) const {
        return operator()(P, lambda, 10 * lambda);
    }

    Result operator()(const MatrixS<M, N> &P, bcg_scalar_t lambda, bcg_scalar_t mu, bcg_scalar_t tol = scalar_eps,
                      int max_iterations = 1000) const {
        MatrixS<M, N> X = P;
        size_t rows = X.rows();
        size_t cols = X.cols();
        Matrix<bool, M, N> mask = X.unaryExpr(
                [](Real v) { return (std::isfinite(v) || std::isnan(v)) ? true : false; });
        X = mask.select(0, X);

        bcg_scalar_t normX = X.array().square().sum();
        Result result;
        result.L = MatrixS<M, N>::Zero(rows, cols);
        result.S = MatrixS<M, N>::Zero(rows, cols);
        MatrixS<M, N> Y = MatrixS<M, N>::Zero(rows, cols);
        MatrixS<M, N> Z;

        for (size_t i = 0; i < max_iterations; ++i) {
            int rank = 0;
            result.L = Do(1.0 / mu, X - result.S + (1.0 / mu) * Y, rank);
            result.S = So(lambda / mu, X - result.L + (1.0 / mu) * Y);
            Z = X - result.L - result.S;
            Z = mask.select(0, Z);
            Y += mu * Z;
            Real error = Z.array().square().sum() / normX;
            if (i == 1 || ((i % 10) == 0) || (error < tol)) {
                std::cout << "iter: " << i << "err: " << error << "rank(L): " << rank << "card(S): "
                          << mask.select(0, 1).sum() << "\n";
            }
            if (error < tol) break;
        }
        return result;
    }

private:
    inline MatrixS<M, N> So(bcg_scalar_t tau, const MatrixS<M, N> &X) {
        return (X.array().sign() * (X.array().abs() - tau).cwiseMax(0));
    }

    inline MatrixS<M, N> Do(bcg_scalar_t tau, const MatrixS<M, N> &X, int &rank) {
        Eigen::BDCSVD<MatrixS<M, N>> svd(X, Eigen::ComputeFullU | Eigen::ComputeFullV);
        rank = svd.rank();
        return (svd.matrixU() * So(tau, svd.singualValues()) * svd.matrixV().transpose());
    }
};

}

#endif //BCG_GRAPHICS_BCG_ROBUST_PCA_H
