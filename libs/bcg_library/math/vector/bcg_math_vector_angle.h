//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATH_VECTOR_ANGLE_H
#define BCG_GRAPHICS_BCG_MATH_VECTOR_ANGLE_H

#include "bcg_math_vector_cos.h"

namespace bcg {

template<int N>
inline bcg_scalar_t vector_angle(const VectorS<N> &v0, const VectorS<N> &v1) {
    return safe_acos(vector_cos(v0, v1));
}

}
#endif //BCG_GRAPHICS_BCG_MATH_VECTOR_ANGLE_H
