//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_ROTATION_CROSS_PRODUCT_MATRIX_H
#define BCG_GRAPHICS_BCG_ROTATION_CROSS_PRODUCT_MATRIX_H

#include "math/bcg_linalg.h"

namespace bcg{

inline MatrixS<3, 3> cross_product_matrix(const VectorS<3> &v) {
    MatrixS<3, 3> result(MatrixS<3, 3>::Zero());
    result(0, 1) = -v[2];
    result(0, 2) = v[1];
    result(1, 0) = v[2];
    result(1, 2) = -v[0];
    result(2, 0) = -v[1];
    result(2, 1) = v[0];
    return result;
}

}

#endif //BCG_GRAPHICS_BCG_ROTATION_CROSS_PRODUCT_MATRIX_H
