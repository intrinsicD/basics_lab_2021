//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATH_VECTOR_SIN_H
#define BCG_GRAPHICS_BCG_MATH_VECTOR_SIN_H

#include "bcg_math_vector_angle.h"

namespace bcg{

template<int N>
inline bcg_scalar_t vector_sin(const VectorS<N> &v0, const VectorS<N> &v1) {
    return std::sin(vector_angle<N>(v0, v1));
}

}

#endif //BCG_GRAPHICS_BCG_MATH_VECTOR_SIN_H
