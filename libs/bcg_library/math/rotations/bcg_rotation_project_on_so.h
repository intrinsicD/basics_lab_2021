//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_ROTATION_PROJECT_ON_SO_H
#define BCG_GRAPHICS_BCG_ROTATION_PROJECT_ON_SO_H

#include "math/bcg_linalg.h"

namespace bcg{

template<int M, int N>
inline MatrixS<M, N> project_on_so(const MatrixS<M, N> &A, bool no_reflections = true) {
    Eigen::BDCSVD<MatrixS<M, N>> svd(A, Eigen::ComputeFullU | Eigen::ComputeFullV);
    VectorS<N> C(VectorS<N>::Ones(A.cols()));
    //take out reflections
    if (no_reflections) {
        C(A.cols() - 1) = svd.matrixU().determinant() * svd.matrixV().transpose().determinant();
    }

    // R = U * V' if A = target' * source
    return svd.matrixU() * C.asDiagonal() * svd.matrixV().transpose();
}

}

#endif //BCG_GRAPHICS_BCG_ROTATION_PROJECT_ON_SO_H
