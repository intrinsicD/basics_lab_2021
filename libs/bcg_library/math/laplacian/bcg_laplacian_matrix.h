//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_LAPLACIAN_MATRIX_H
#define BCG_GRAPHICS_BCG_LAPLACIAN_MATRIX_H

#include "math/sparse_matrix/bcg_sparse_matrix.h"

namespace bcg{

struct laplacian_matrix {
    SparseMatrix<bcg_scalar_t> S, M;

    SparseMatrix<bcg_scalar_t> normalize() const {
        return M.cwiseInverse() * S;
    }

    SparseMatrix<bcg_scalar_t> symmetric() const {
        return M.cwiseSqrt().cwiseInverse() * S * M.cwiseSqrt().cwiseInverse();
    }
};

}

#endif //BCG_GRAPHICS_BCG_LAPLACIAN_MATRIX_H
