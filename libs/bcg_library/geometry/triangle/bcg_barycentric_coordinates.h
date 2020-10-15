//
// Created by alex on 13.10.20.
//

#ifndef BCG_GRAPHICS_BCG_BARYCENTRIC_COORDINATES_H
#define BCG_GRAPHICS_BCG_BARYCENTRIC_COORDINATES_H

#include <array>
#include "math/bcg_linalg.h"

namespace bcg {

template<bcg_index_t N>
struct triangle;

template<bcg_index_t N>
inline VectorS<3> to_barycentric_coordinates(const triangle<N> &t, const VectorS<N> &point) {
    VectorS<N> v0 = t.edge0();
    VectorS<N> v1 = -t.edge2();
    VectorS<N> v2 = point - t.points[0];

    bcg_scalar_t d00 = v0.dot(v0);
    bcg_scalar_t d01 = v0.dot(v1);
    bcg_scalar_t d11 = v1.dot(v1);
    bcg_scalar_t d20 = v2.dot(v0);
    bcg_scalar_t d21 = v2.dot(v1);

    bcg_scalar_t denom = d00 * d11 - d01 * d01;

    if (denom < scalar_eps) {
        return zero3s;
    }

    VectorS<3> result;
    result[1] = (d11 * d20 - d01 * d21) / denom;
    result[2] = (d00 * d21 - d01 * d20) / denom;
    result[0] = 1.0 - result[1] - result[2];

    return result;
}

template<bcg_index_t N>
inline VectorS<N>
from_barycentric_coords(const triangle<N> &t, const VectorS<3> &bc) {
    return VectorS<N>(t.points[0] * bc[0] + t.points[1] * bc[1] + t.points[2] * bc[2]);
}

template<bcg_index_t N>
VectorS<N>
from_barycentric_coords(const triangle<N> &t, bcg_scalar_t b0, bcg_scalar_t b1, bcg_scalar_t b2) {
    return from_barycentric_coords(t, {b0, b1, b2});
}

}

#endif //BCG_GRAPHICS_BCG_BARYCENTRIC_COORDINATES_H
