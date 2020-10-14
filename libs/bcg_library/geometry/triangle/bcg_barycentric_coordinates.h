//
// Created by alex on 13.10.20.
//

#ifndef BCG_GRAPHICS_BCG_BARYCENTRIC_COORDINATES_H
#define BCG_GRAPHICS_BCG_BARYCENTRIC_COORDINATES_H

#include <array>
#include "../../math/bcg_linalg.h"

namespace bcg {

template<int N>
struct triangle;

template<int N>
vec3f barycentric_coordinates(const triangle<N> &t, const vec<N, float> &point) {
    auto v0 = t.points[0] - t.points[1];
    auto v1 = t.points[2] - t.points[1];
    auto v2 = point - t.points[0];

    auto d00 = v0.dot(v0);
    auto d01 = v0.dot(v1);
    auto d11 = v1.dot(v1);
    auto d20 = v2.dot(v0);
    auto d21 = v2.dot(v1);

    auto denom = d00 * d11 - d01 * d01;

    if (denom < flt_eps) {
        return zero3f;
    }

    vec3f result;
    result[1] = (d11 * d20 - d01 * d21) / denom;
    result[2] = (d00 * d21 - d01 * d20) / denom;
    result[0] = 1.0 - result[1] - result[2];

    return result;
}

template<int N>
vec<N, float>
from_barycentric_coords(const triangle<N> &t, const vec3f &bc) {
    return {t.points[0] * bc[0] + t.points[1] * bc[1] + t.points[2] * bc[2]};
}

template<int N>
vec<N, float>
from_barycentric_coords(const triangle<N> &t, float b0, float b1, float b2) {
    return from_barycentric_coords(t, {b0, b1, b2});
}

}

#endif //BCG_GRAPHICS_BCG_BARYCENTRIC_COORDINATES_H
