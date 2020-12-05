//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_ROTATION_MATRIX_EXPONENTIAL_H
#define BCG_GRAPHICS_BCG_ROTATION_MATRIX_EXPONENTIAL_H

#include "bcg_rotation_cross_product_matrix.h"

namespace bcg {

inline MatrixS<3, 3> matrix_exponential(const VectorS<3> &v) {
    //Rodrigues' fromula
    bcg_scalar_t theta(v.norm());
    if (theta == 0) return MatrixS<3, 3>::Identity();
    MatrixS<3, 3> rot(cross_product_matrix((v / theta).eval()));
    return MatrixS<3, 3>::Identity() + std::sin(theta) * rot + (1.0f - std::cos(theta)) * rot * rot;
}

}

#endif //BCG_GRAPHICS_BCG_ROTATION_MATRIX_EXPONENTIAL_H
