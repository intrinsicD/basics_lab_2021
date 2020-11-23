//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATRIX_HORIZONTAL_STACK_H
#define BCG_GRAPHICS_BCG_MATRIX_HORIZONTAL_STACK_H

#include "bcg_matrix.h"

namespace bcg{

template<int N, typename Real>
Matrix<Real, -1, N>
horizontal_stack(const std::vector<Matrix<Real, -1, N>, Eigen::aligned_allocator<Matrix<Real, -1, N>>> &mats) {
    if (mats.size() == 0) throw std::logic_error("must have at least one matrix to stack");

    long nRows = mats[0].rows();

    size_t nColsTot = 0;

    for (const Matrix<Real, -1, N> &mat : mats) {
        if (mat.rows() != nRows) throw std::logic_error("all matrices must have same row size");

        nColsTot += mat.cols();
    }

    Matrix<Real, -1, N> result(nRows, nColsTot);

    size_t nColsCurr = 0;
    for (const Matrix<Real, -1, N> &mat : mats) {
        result.block(0, nColsCurr, mat.rows(), nColsCurr + mat.cols()) = mat;
        nColsCurr += mat.cols();
    }

    return result;
}

}

#endif //BCG_GRAPHICS_BCG_MATRIX_HORIZONTAL_STACK_H
