//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_ROTATION_MATRIX_LOGARITHM_H
#define BCG_GRAPHICS_BCG_ROTATION_MATRIX_LOGARITHM_H

#include "math/bcg_linalg.h"
#include "math/bcg_math_common.h"

namespace bcg{

inline VectorS<3> matrix_logarithm(const MatrixS<3, 3> &R) {
    MatrixS<3, 3> anti_symmetric_part((R - R.transpose()) / 2.0);
    VectorS<3> y(anti_symmetric_part(2, 1), anti_symmetric_part(0, 2), anti_symmetric_part(1, 0));
    assert(!y.array().isNaN().any());
    bcg_scalar_t theta = y.norm();
    if (theta < scalar_eps) return VectorS<3>::Zero();
    VectorS<3> result = safe_asin(theta) * y.array() / theta;
    assert(!y.array().isNaN().any());
    assert(!std::isnan(theta));
    assert(!result.array().isNaN().any());
    return result;
}


}
#endif //BCG_GRAPHICS_BCG_ROTATION_MATRIX_LOGARITHM_H
