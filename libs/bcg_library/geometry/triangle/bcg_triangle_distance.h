//
// Created by alex on 13.10.20.
//

#ifndef BCG_GRAPHICS_BCG_TRIANGLE_DISTANCE_H
#define BCG_GRAPHICS_BCG_TRIANGLE_DISTANCE_H

#include "bcg_triangle.h"

namespace bcg {

inline bcg_scalar_t signed_distance(const triangle3 &t, const VectorS<3> &point) {
    return (point - t.points[0]).dot(normal(t));
}

inline bcg_scalar_t distance(const triangle3 &t, const VectorS<3> &point) {
    return std::abs(signed_distance(t, point));
}

inline bcg_scalar_t squared_distance(const triangle3 &t, const VectorS<3> &point) {
    return std::pow(signed_distance(t, point), 2);
}

}

#endif //BCG_GRAPHICS_BCG_TRIANGLE_DISTANCE_H
