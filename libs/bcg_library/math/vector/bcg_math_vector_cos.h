//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATH_VECTOR_COS_H
#define BCG_GRAPHICS_BCG_MATH_VECTOR_COS_H

#include "math/bcg_linalg.h"
#include "math/bcg_math_common.h"

namespace bcg{

template<int N>
inline bcg_scalar_t vector_cos(const VectorS<N> &v0, const VectorS<N> &v1) {
    return v0.dot(v1) / (v0.norm() * v1.norm());
}

}

#endif //BCG_GRAPHICS_BCG_MATH_VECTOR_COS_H
