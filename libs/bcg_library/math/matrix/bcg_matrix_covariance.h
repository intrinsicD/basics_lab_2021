//
// Created by alex on 24.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATRIX_COVARIANCE_H
#define BCG_GRAPHICS_BCG_MATRIX_COVARIANCE_H

#include "bcg_matrix.h"
#include "math/vector/bcg_vector.h"

namespace bcg{

template<int M, int D>
inline MatrixS<D, D> covariance(const MatrixS<M, D> &mat1, const MatrixS<M, D> &mat2){
    return mat1.transpose() * mat2 / (mat1.rows() - 1);
}

template<int M, int D>
inline MatrixS<D, D> covariance(const MatrixS<M, D> &mat1, const VectorS<M> &weights, const MatrixS<M, D> &mat2){
    return mat1.transpose() * weights.asDiagoanl() * mat2  / weights.sum();
}

}

#endif //BCG_GRAPHICS_BCG_MATRIX_COVARIANCE_H
