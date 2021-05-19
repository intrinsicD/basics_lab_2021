//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_SPARSE_MATRIX_H
#define BCG_GRAPHICS_BCG_SPARSE_MATRIX_H

#include "math/bcg_math_types.h"
#include "Eigen/SparseCore"

namespace bcg{

template<typename T>
using SparseMatrix = Eigen::SparseMatrix<T>;

}

#endif //BCG_GRAPHICS_BCG_SPARSE_MATRIX_H
