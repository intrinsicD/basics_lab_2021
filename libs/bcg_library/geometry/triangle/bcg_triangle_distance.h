//
// Created by alex on 13.10.20.
//

#ifndef BCG_GRAPHICS_BCG_TRIANGLE_DISTANCE_H
#define BCG_GRAPHICS_BCG_TRIANGLE_DISTANCE_H

#include "bcg_triangle.h"

namespace bcg {

inline float signed_distance(const triangle3 &t, const vec3f &point) {
    return dot(point - t.points[0], normal(t));
}

inline float distance(const triangle3 &t, const vec3f &point) {
    return std::abs(signed_distance(t, point));
}

inline float squared_distance(const triangle3 &t, const vec3f &point) {
    return std::pow(signed_distance(t, point), 2);
}

}

#endif //BCG_GRAPHICS_BCG_TRIANGLE_DISTANCE_H
