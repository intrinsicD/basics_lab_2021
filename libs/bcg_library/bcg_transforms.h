//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_TRANSFORMS_H
#define BCG_GRAPHICS_BCG_TRANSFORMS_H

#include "bcg_vector.h"
#include "bcg_matrix.h"

namespace bcg {

// Transforms points, vectors and directions by matrices.
inline vec2f transform_point(const mat3f &a, const vec2f &b);

inline vec2f transform_vector(const mat3f &a, const vec2f &b);

inline vec2f transform_direction(const mat3f &a, const vec2f &b);

inline vec2f transform_normal(const mat3f &a, const vec2f &b);

inline vec2f transform_vector(const mat2f &a, const vec2f &b);

inline vec2f transform_direction(const mat2f &a, const vec2f &b);

inline vec2f transform_normal(const mat2f &a, const vec2f &b);

inline vec3f transform_point(const mat4f &a, const vec3f &b);

inline vec3f transform_vector(const mat4f &a, const vec3f &b);

inline vec3f transform_direction(const mat4f &a, const vec3f &b);

inline vec3f transform_vector(const mat3f &a, const vec3f &b);

inline vec3f transform_direction(const mat3f &a, const vec3f &b);

inline vec3f transform_normal(const mat3f &a, const vec3f &b);

}

// -----------------------------------------------------------------------------
//
//
// IMPLEMENTATION
//
//
// -----------------------------------------------------------------------------

namespace bcg {

// Transforms points, vectors and directions by matrices.
inline vec2f transform_point(const mat3f &a, const vec2f &b) {
    auto tvb = a * vec3f{b.x, b.y, 1};
    return vec2f{tvb.x, tvb.y} / tvb.z;
}

inline vec2f transform_vector(const mat3f &a, const vec2f &b) {
    auto tvb = a * vec3f{b.x, b.y, 0};
    return vec2f{tvb.x, tvb.y} / tvb.z;
}

inline vec2f transform_direction(const mat3f &a, const vec2f &b) {
    return normalize(transform_vector(a, b));
}

inline vec2f transform_normal(const mat3f &a, const vec2f &b) {
    return normalize(transform_vector(transpose(inverse(a)), b));
}

inline vec2f transform_vector(const mat2f &a, const vec2f &b) {
    return a * b;
}

inline vec2f transform_direction(const mat2f &a, const vec2f &b) {
    return normalize(transform_vector(a, b));
}

inline vec2f transform_normal(const mat2f &a, const vec2f &b) {
    return normalize(transform_vector(transpose(inverse(a)), b));
}

inline vec3f transform_point(const mat4f &a, const vec3f &b) {
    auto tvb = a * vec4f{b.x, b.y, b.z, 1};
    return vec3f{tvb.x, tvb.y, tvb.z} / tvb.w;
}

inline vec3f transform_vector(const mat4f &a, const vec3f &b) {
    auto tvb = a * vec4f{b.x, b.y, b.z, 0};
    return vec3f{tvb.x, tvb.y, tvb.z};
}

inline vec3f transform_direction(const mat4f &a, const vec3f &b) {
    return normalize(transform_vector(a, b));
}

inline vec3f transform_vector(const mat3f &a, const vec3f &b) {
    return a * b;
}

inline vec3f transform_direction(const mat3f &a, const vec3f &b) {
    return normalize(transform_vector(a, b));
}

inline vec3f transform_normal(const mat3f &a, const vec3f &b) {
    return normalize(transform_vector(transpose(inverse(a)), b));
}

}

#endif //BCG_GRAPHICS_BCG_TRANSFORMS_H
