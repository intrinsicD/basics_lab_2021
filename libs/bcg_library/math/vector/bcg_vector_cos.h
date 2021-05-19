//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_VECTOR_COS_H
#define BCG_GRAPHICS_BCG_VECTOR_COS_H

#include "math/bcg_linalg.h"
#include "math/bcg_math_types.h"

namespace bcg{

inline bcg_scalar_t clamp_cos(bcg_scalar_t v) {
    const bcg_scalar_t bound = 0.9986; // 3 degrees
    return (v < -bound ? -bound : (v > bound ? bound : v));
}

template<int N>
inline bcg_scalar_t vector_cos(const VectorS<N> &v0, const VectorS<N> &v1) {
    return v0.dot(v1) / (v0.norm() * v1.norm());
}

}

#endif //BCG_GRAPHICS_BCG_VECTOR_COS_H
