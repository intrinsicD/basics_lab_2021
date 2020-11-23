//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_SPARSE_VERTICAL_STACK_H
#define BCG_GRAPHICS_BCG_SPARSE_VERTICAL_STACK_H

#include "bcg_sparse_matrix.h"

namespace bcg {

SparseMatrix<bcg_scalar_t>
vertical_stack(
        const std::vector<SparseMatrix<bcg_scalar_t>, Eigen::aligned_allocator<SparseMatrix<bcg_scalar_t>>> &mats);

}

#endif //BCG_GRAPHICS_BCG_SPARSE_VERTICAL_STACK_H
