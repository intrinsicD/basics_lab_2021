//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATRIX_VERTICAL_STACK_H
#define BCG_GRAPHICS_BCG_MATRIX_VERTICAL_STACK_H

#include "bcg_matrix.h"

namespace bcg{

template<int M, typename Real>
Matrix<Real, M, -1>
vertical_stack(const std::vector<Matrix<Real, M, -1>, Eigen::aligned_allocator<Matrix<Real, M, -1>>> &mats) {
    if (mats.size() == 0) throw std::logic_error("must have at least one matrix to stack");

    long nCols = mats[0].cols();

    size_t nRowsTot = 0;

    for (const Matrix<Real, M, -1> &mat : mats) {
        if (mat.cols() != nCols) throw std::logic_error("all matrices must have same col size");

        nRowsTot += mat.rows();
    }

    Matrix<Real, M, -1> result(nRowsTot, nCols);

    size_t nRowsCurr = 0;
    for (const Matrix<Real, M, -1> &mat : mats) {
        result.block(nRowsCurr, 0, nRowsCurr + mat.rows(), nCols) = mat;
        nRowsCurr += mat.rows();
    }

    return result;
}

}

#endif //BCG_GRAPHICS_BCG_MATRIX_VERTICAL_STACK_H
