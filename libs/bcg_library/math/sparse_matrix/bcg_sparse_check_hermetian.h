//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_SPARSE_CHECK_HERMETIAN_H
#define BCG_GRAPHICS_BCG_SPARSE_CHECK_HERMETIAN_H

#include "bcg_sparse_matrix.h"

namespace bcg{

void check_hermetrian(const SparseMatrix<bcg_scalar_t> &m, bcg_scalar_t eps = scalar_eps);

}

#endif //BCG_GRAPHICS_BCG_SPARSE_CHECK_HERMETIAN_H
