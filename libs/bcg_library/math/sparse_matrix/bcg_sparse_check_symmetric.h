//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_SPARSE_CHECK_SYMMETRIC_H
#define BCG_GRAPHICS_BCG_SPARSE_CHECK_SYMMETRIC_H

#include "bcg_sparse_matrix.h"

namespace bcg{

void check_symmetric(const SparseMatrix<bcg_scalar_t> &m, bcg_scalar_t eps = scalar_eps);

}

#endif //BCG_GRAPHICS_BCG_SPARSE_CHECK_SYMMETRIC_H
