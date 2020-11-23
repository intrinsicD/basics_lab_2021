//
// Created by alex on 23.11.20.
//

#include "bcg_sparse_check_finite.h"

namespace bcg{

void check_finite(const SparseMatrix<bcg_scalar_t> &m) {
    for (int k = 0; k < m.outerSize(); ++k) {
        for (typename SparseMatrix<bcg_scalar_t>::InnerIterator it(m, k); it; ++it) {
            if (!std::isfinite(it.value())) {
                std::ostringstream msg;
                msg << "checkFinite() failure: Non-finite entry [" << it.row() << "," << it.col() << "] = "
                    << it.value();
                throw std::logic_error(msg.str());
            }
        }
    }
}

}