//
// Created by alex on 23.11.20.
//


#include "bcg_sparse_horizontal_stack.h"

namespace bcg {

SparseMatrix<bcg_scalar_t>
horizontal_stack(
        const std::vector<SparseMatrix<bcg_scalar_t>, Eigen::aligned_allocator<SparseMatrix<bcg_scalar_t>>> &mats) {
    if (mats.size() == 0) throw std::logic_error("must have at least one matrix to stack");

    std::vector<Eigen::Triplet<bcg_scalar_t>> triplets;
    long nRows = mats[0].rows();

    size_t nColsTot = 0;

    for (const SparseMatrix<bcg_scalar_t> &mat : mats) {

        if (mat.rows() != nRows) throw std::logic_error("all matrices must have same row size");

        // Copy entries
        for (int k = 0; k < mat.outerSize(); ++k) {
            for (typename SparseMatrix<bcg_scalar_t>::InnerIterator it(mat, k); it; ++it) {
                triplets.emplace_back(it.row(), it.col() + nColsTot, it.value());
            }
        }

        nColsTot += mat.cols();
    }

    // Build the matrix
    SparseMatrix<bcg_scalar_t> result(nRows, nColsTot);
    result.setFromTriplets(triplets.begin(), triplets.end());

    return result;
}

}