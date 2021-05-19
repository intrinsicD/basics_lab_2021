//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATRIX_H
#define BCG_GRAPHICS_BCG_MATRIX_H

#include "Eigen/Core"
#include "math/bcg_math_types.h"

namespace bcg{

template<typename T, int M, int N, int Options=0>
using Matrix = Eigen::Matrix<T, M, N, Options, M, N>;

template<int M, int N>
using MatrixS = Matrix<bcg_scalar_t, M, N>;

}

#endif //BCG_GRAPHICS_BCG_MATRIX_H
