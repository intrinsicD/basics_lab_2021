//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATRIX_CHECK_FINITE_H
#define BCG_GRAPHICS_BCG_MATRIX_CHECK_FINITE_H

#include "bcg_matrix.h"

namespace bcg {

template<int M, int N, typename Real>
inline void check_finite(const Matrix<Real, M, N> &mat) {
    for (unsigned int i = 0; i < mat.rows(); ++i) {
        for (unsigned int j = 0; j < mat.cols(); ++j) {
            if (!std::isfinite(mat(i, j))) {
                std::ostringstream msg;
                msg << "checkFinite() failure. Non-finite entry [" << i << "," << j << "] = " << mat(i, j);
                throw std::logic_error(msg.str());
            }
        }
    }
}

}

#endif //BCG_GRAPHICS_BCG_MATRIX_CHECK_FINITE_H
