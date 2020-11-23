//
// Created by alex on 23.11.20.
//


#include "bcg_sparse_vertical_stack.h"

namespace bcg {

SparseMatrix<bcg_scalar_t>
vertical_stack(
        const std::vector<SparseMatrix<bcg_scalar_t>, Eigen::aligned_allocator<SparseMatrix<bcg_scalar_t>>> &mats) {
    if (mats.size() == 0) throw std::logic_error("must have at least one matrix to stack");

    std::vector<Eigen::Triplet<bcg_scalar_t>> triplets;
    long nCols = mats[0].cols();

    size_t nRowsTot = 0;

    for (const SparseMatrix<bcg_scalar_t> &mat : mats) {

        if (mat.cols() != nCols) throw std::logic_error("all matrices must have same column size");

        // Copy entries
        for (int k = 0; k < mat.outerSize(); ++k) {
            for (typename SparseMatrix<bcg_scalar_t>::InnerIterator it(mat, k); it; ++it) {
                triplets.emplace_back(it.row() + nRowsTot, it.col(), it.value());
            }
        }

        nRowsTot += mat.rows();
    }

    // Build the matrix
    SparseMatrix<bcg_scalar_t> result(nRowsTot, nCols);
    result.setFromTriplets(triplets.begin(), triplets.end());

    return result;
}

}