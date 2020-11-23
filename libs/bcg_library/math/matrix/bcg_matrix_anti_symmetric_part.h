//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATRIX_ANTI_SYMMETRIC_PART_H
#define BCG_GRAPHICS_BCG_MATRIX_ANTI_SYMMETRIC_PART_H

#include "bcg_matrix.h"

namespace bcg {

template<int N, typename Real>
inline Matrix<Real, N, N> AntiSymmetricPart(const Matrix<Real, N, N> &A) {
    return (A - A.transpose()) / 2.0;
}

}

#endif //BCG_GRAPHICS_BCG_MATRIX_ANTI_SYMMETRIC_PART_H
