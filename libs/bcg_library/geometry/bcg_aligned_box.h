//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_ALIGNED_BOX_H
#define BCG_GRAPHICS_BCG_ALIGNED_BOX_H

// -----------------------------------------------------------------------------
// INCLUDES
// -----------------------------------------------------------------------------

#include <initializer_list>
#include "bcg_library/math/bcg_math.h"
#include "bcg_transforms.h"

// -----------------------------------------------------------------------------
// AXIS ALIGNED BOUNDING BOXES
// -----------------------------------------------------------------------------

namespace bcg {

// Axis aligned bounding box represented as a min/max vector pairs.
struct bbox2f {
    vec2f min = {flt_max, flt_max};
    vec2f max = {flt_min, flt_min};

    vec2f &operator[](int i);

    const vec2f &operator[](int i) const;
};

// Axis aligned bounding box represented as a min/max vector pairs.
struct bbox3f {
    vec3f min = {flt_max, flt_max, flt_max};
    vec3f max = {flt_min, flt_min, flt_min};

    vec3f &operator[](int i);

    const vec3f &operator[](int i) const;
};

// Empty bbox constant.
inline const auto invalidb2f = bbox2f{};
inline const auto invalidb3f = bbox3f{};

// Bounding box properties
inline vec2f center(const bbox2f &a);

inline vec2f size(const bbox2f &a);

// Bounding box comparisons.
inline bool operator==(const bbox2f &a, const bbox2f &b);

inline bool operator!=(const bbox2f &a, const bbox2f &b);

// Bounding box expansions with points and other boxes.
inline bbox2f merge(const bbox2f &a, const vec2f &b);

inline bbox2f merge(const bbox2f &a, const bbox2f &b);

inline void expand(bbox2f &a, const vec2f &b);

inline void expand(bbox2f &a, const bbox2f &b);

// Bounding box properties
inline vec3f center(const bbox3f &a);

inline vec3f size(const bbox3f &a);

// Bounding box comparisons.
inline bool operator==(const bbox3f &a, const bbox3f &b);

inline bool operator!=(const bbox3f &a, const bbox3f &b);

// Bounding box expansions with points and other boxes.
inline bbox3f merge(const bbox3f &a, const vec3f &b);

inline bbox3f merge(const bbox3f &a, const bbox3f &b);

inline void expand(bbox3f &a, const vec3f &b);

inline void expand(bbox3f &a, const bbox3f &b);

}

// -----------------------------------------------------------------------------
// TRANSFORMS
// -----------------------------------------------------------------------------
namespace bcg {
// Transforms bounding boxes by matrices.
inline bbox3f transform_bbox(const mat4f &a, const bbox3f &b);

}

// -----------------------------------------------------------------------------
// PRIMITIVE BOUNDS
// -----------------------------------------------------------------------------
namespace bcg {

// Primitive bounds.
inline bbox3f point_bounds(const vec3f &p);

inline bbox3f point_bounds(const vec3f &p, float r);

inline bbox3f line_bounds(const vec3f &p0, const vec3f &p1);

inline bbox3f line_bounds(const vec3f &p0, const vec3f &p1, float r0, float r1);

inline bbox3f triangle_bounds(const vec3f &p0, const vec3f &p1, const vec3f &p2);

inline bbox3f quad_bounds(const vec3f &p0, const vec3f &p1, const vec3f &p2, const vec3f &p3);

}

// -----------------------------------------------------------------------------
//
//
// IMPLEMENTATION
//
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// AXIS ALIGNED BOUNDING BOXES
// -----------------------------------------------------------------------------
namespace bcg {

// Axis aligned bounding box represented as a min/max vector pairs.
inline vec2f &bbox2f::operator[](int i) { return (&min)[i]; }

inline const vec2f &bbox2f::operator[](int i) const { return (&min)[i]; }

// Axis aligned bounding box represented as a min/max vector pairs.
inline vec3f &bbox3f::operator[](int i) { return (&min)[i]; }

inline const vec3f &bbox3f::operator[](int i) const { return (&min)[i]; }

// Bounding box properties
inline vec2f center(const bbox2f &a) { return (a.min + a.max) / 2; }

inline vec2f size(const bbox2f &a) { return a.max - a.min; }

// Bounding box comparisons.
inline bool operator==(const bbox2f &a, const bbox2f &b) {
    return a.min == b.min && a.max == b.max;
}

inline bool operator!=(const bbox2f &a, const bbox2f &b) {
    return a.min != b.min || a.max != b.max;
}

// Bounding box expansions with points and other boxes.
inline bbox2f merge(const bbox2f &a, const vec2f &b) {
    return {min(a.min, b), max(a.max, b)};
}

inline bbox2f merge(const bbox2f &a, const bbox2f &b) {
    return {min(a.min, b.min), max(a.max, b.max)};
}

inline void expand(bbox2f &a, const vec2f &b) { a = merge(a, b); }

inline void expand(bbox2f &a, const bbox2f &b) { a = merge(a, b); }

// Bounding box properties
inline vec3f center(const bbox3f &a) { return (a.min + a.max) / 2; }

inline vec3f size(const bbox3f &a) { return a.max - a.min; }

// Bounding box comparisons.
inline bool operator==(const bbox3f &a, const bbox3f &b) {
    return a.min == b.min && a.max == b.max;
}

inline bool operator!=(const bbox3f &a, const bbox3f &b) {
    return a.min != b.min || a.max != b.max;
}

// Bounding box expansions with points and other boxes.
inline bbox3f merge(const bbox3f &a, const vec3f &b) {
    return {min(a.min, b), max(a.max, b)};
}

inline bbox3f merge(const bbox3f &a, const bbox3f &b) {
    return {min(a.min, b.min), max(a.max, b.max)};
}

inline void expand(bbox3f &a, const vec3f &b) { a = merge(a, b); }

inline void expand(bbox3f &a, const bbox3f &b) { a = merge(a, b); }

}  // namespace bcg

// -----------------------------------------------------------------------------
// TRANSFORMS
// -----------------------------------------------------------------------------
namespace bcg {

inline bbox3f transform_bbox(const mat4f &a, const bbox3f &b) {
    auto corners = {vec3f{b.min.x, b.min.y, b.min.z},
                    vec3f{b.min.x, b.min.y, b.max.z}, vec3f{b.min.x, b.max.y, b.min.z},
                    vec3f{b.min.x, b.max.y, b.max.z}, vec3f{b.max.x, b.min.y, b.min.z},
                    vec3f{b.max.x, b.min.y, b.max.z}, vec3f{b.max.x, b.max.y, b.min.z},
                    vec3f{b.max.x, b.max.y, b.max.z}};
    auto xformed = bbox3f();
    for (auto &corner : corners)
        xformed = merge(xformed, transform_point(a, corner));
    return xformed;
}

}  // namespace bcg


// -----------------------------------------------------------------------------
// PRIMITIVE BOUNDS
// -----------------------------------------------------------------------------
namespace bcg {

// Primitive bounds.
inline bbox3f point_bounds(const vec3f &p) { return {p, p}; }

inline bbox3f point_bounds(const vec3f &p, float r) {
    return {min(p - r, p + r), max(p - r, p + r)};
}

inline bbox3f line_bounds(const vec3f &p0, const vec3f &p1) {
    return {min(p0, p1), max(p0, p1)};
}

inline bbox3f line_bounds(
        const vec3f &p0, const vec3f &p1, float r0, float r1) {
    return {min(p0 - r0, p1 - r1), max(p0 + r0, p1 + r1)};
}

inline bbox3f triangle_bounds(
        const vec3f &p0, const vec3f &p1, const vec3f &p2) {
    return {min(p0, min(p1, p2)), max(p0, max(p1, p2))};
}

inline bbox3f quad_bounds(
        const vec3f &p0, const vec3f &p1, const vec3f &p2, const vec3f &p3) {
    return {min(p0, min(p1, min(p2, p3))), max(p0, max(p1, max(p2, p3)))};
}

}  // namespace bcg
#endif //BCG_GRAPHICS_BCG_ALIGNED_BOX_H
