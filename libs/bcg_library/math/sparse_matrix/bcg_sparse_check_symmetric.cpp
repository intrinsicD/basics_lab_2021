//
// Created by alex on 23.11.20.
//

#include "bcg_sparse_check_symmetric.h"

namespace bcg{

void check_symmetric(const SparseMatrix<bcg_scalar_t> &m, bcg_scalar_t eps) {
    // Compute a scale factor for the matrix to use for closeness tests
    if (eps == -1.) {
        double sum = 0;
        size_t nEntries = 0;
        for (int k = 0; k < m.outerSize(); ++k) {
            for (typename SparseMatrix<bcg_scalar_t>::InnerIterator it(m, k); it; ++it) {
                sum += std::abs(it.value());
                nEntries++;
            }
        }
        auto scale = sum / nEntries;
        eps = scale * 1e-8;
    }

    // Test each symmtric pair in the matrix (actually tests each twice)
    for (int k = 0; k < m.outerSize(); ++k) {
        for (typename SparseMatrix<bcg_scalar_t>::InnerIterator it(m, k); it; ++it) {

            auto thisVal = it.value();
            auto otherVal = m.coeff(it.col(), it.row());

            if (std::abs(thisVal - otherVal) > eps) {
                std::ostringstream msg;
                msg << "checkSymmetric() error. Non-symmtric matrix entry at [" << it.row() << "," << it.col() << "]."
                    << "    [" << it.row() << "," << it.col() << "] = " << thisVal
                    << "    [" << it.col() << "," << it.row() << "] = " << otherVal;
                throw std::logic_error(msg.str());
            }
        }
    }
}

}