//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_RAY_H
#define BCG_GRAPHICS_BCG_RAY_H

// -----------------------------------------------------------------------------
// INCLUDES
// -----------------------------------------------------------------------------

#include "bcg_math.h"

namespace bcg {


// Ray epsilon
inline const auto ray_eps = 1e-4f;

struct ray2f {
    vec2f o = {0, 0};
    vec2f d = {0, 1};
    float tmin = ray_eps;
    float tmax = flt_max;
};

// Rays with origin, direction and min/max t value.
struct ray3f {
    vec3f o = {0, 0, 0};
    vec3f d = {0, 0, 1};
    float tmin = ray_eps;
    float tmax = flt_max;
};

// Computes a point on a ray
inline vec2f ray_point(const ray2f &ray, float t);

inline vec3f ray_point(const ray3f &ray, float t);

}

// -----------------------------------------------------------------------------
// TRANSFORMS
// -----------------------------------------------------------------------------

namespace bcg {

// Transforms rays.
inline ray3f transform_ray(const mat4f &a, const ray3f &b);

inline ray3f transform_ray(const frame3f &a, const ray3f &b);

}

// -----------------------------------------------------------------------------
//
//
// IMPLEMENTATION
//
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// RAYS
// -----------------------------------------------------------------------------
namespace bcg {

// Computes a point on a ray
inline vec2f ray_point(const ray2f &ray, float t) { return ray.o + ray.d * t; }

inline vec3f ray_point(const ray3f &ray, float t) { return ray.o + ray.d * t; }

}  // namespace bcg


// -----------------------------------------------------------------------------
// TRANSFORMS
// -----------------------------------------------------------------------------
namespace yocto {

// Transforms rays and bounding boxes by matrices.
inline ray3f transform_ray(const mat4f &a, const ray3f &b) {
    return {transform_point(a, b.o), transform_vector(a, b.d), b.tmin, b.tmax};
}

inline ray3f transform_ray(const frame3f &a, const ray3f &b) {
    return {transform_point(a, b.o), transform_vector(a, b.d), b.tmin, b.tmax};
}
}

#endif //BCG_GRAPHICS_BCG_RAY_H
