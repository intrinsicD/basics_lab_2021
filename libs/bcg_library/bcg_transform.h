//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_TRANSFORM_H
#define BCG_GRAPHICS_BCG_TRANSFORM_H


// -----------------------------------------------------------------------------
// TRANSFORMS
// -----------------------------------------------------------------------------
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

// Transforms points, vectors and directions by frames.
inline vec2f transform_point(const frame2f &a, const vec2f &b);

inline vec2f transform_vector(const frame2f &a, const vec2f &b);

inline vec2f transform_direction(const frame2f &a, const vec2f &b);

inline vec2f transform_normal(
        const frame2f &a, const vec2f &b, bool non_rigid = false);

// Transforms points, vectors and directions by frames.
inline vec3f transform_point(const frame3f &a, const vec3f &b);

inline vec3f transform_vector(const frame3f &a, const vec3f &b);

inline vec3f transform_direction(const frame3f &a, const vec3f &b);

inline vec3f transform_normal(
        const frame3f &a, const vec3f &b, bool non_rigid = false);

// Translation, scaling and rotations transforms.
inline frame3f translation_frame(const vec3f &a);

inline frame3f scaling_frame(const vec3f &a);

inline frame3f rotation_frame(const vec3f &axis, float angle);

inline frame3f rotation_frame(const vec4f &quat);

inline frame3f rotation_frame(const quat4f &quat);

inline frame3f rotation_frame(const mat3f &rot);

// Lookat frame. Z-axis can be inverted with inv_xz.
inline frame3f lookat_frame(const vec3f &eye, const vec3f &center,
                            const vec3f &up, bool inv_xz = false);

// OpenGL frustum, ortho and perspecgive matrices.
inline mat4f frustum_mat(float l, float r, float b, float t, float n, float f);

inline mat4f ortho_mat(float l, float r, float b, float t, float n, float f);

inline mat4f ortho2d_mat(float left, float right, float bottom, float top);

inline mat4f ortho_mat(float xmag, float ymag, float near, float far);

inline mat4f perspective_mat(float fovy, float aspect, float near, float far);

inline mat4f perspective_mat(float fovy, float aspect, float near);

// Rotation conversions.
inline std::pair<vec3f, float> rotation_axisangle(const vec4f &quat);

inline vec4f rotation_quat(const vec3f &axis, float angle);

inline vec4f rotation_quat(const vec4f &axisangle);

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

inline vec2f transform_vector(const mat2f &a, const vec2f &b) { return a * b; }

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

inline vec3f transform_vector(const mat3f &a, const vec3f &b) { return a * b; }

inline vec3f transform_direction(const mat3f &a, const vec3f &b) {
    return normalize(transform_vector(a, b));
}

inline vec3f transform_normal(const mat3f &a, const vec3f &b) {
    return normalize(transform_vector(transpose(inverse(a)), b));
}

// Transforms points, vectors and directions by frames.
inline vec2f transform_point(const frame2f &a, const vec2f &b) {
    return a.x * b.x + a.y * b.y + a.o;
}

inline vec2f transform_vector(const frame2f &a, const vec2f &b) {
    return a.x * b.x + a.y * b.y;
}

inline vec2f transform_direction(const frame2f &a, const vec2f &b) {
    return normalize(transform_vector(a, b));
}

inline vec2f transform_normal(
        const frame2f &a, const vec2f &b, bool non_rigid) {
    if (non_rigid) {
        return transform_normal(rotation(a), b);
    } else {
        return normalize(transform_vector(a, b));
    }
}

// Transforms points, vectors and directions by frames.
inline vec3f transform_point(const frame3f &a, const vec3f &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.o;
}

inline vec3f transform_vector(const frame3f &a, const vec3f &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline vec3f transform_direction(const frame3f &a, const vec3f &b) {
    return normalize(transform_vector(a, b));
}

inline vec3f transform_normal(
        const frame3f &a, const vec3f &b, bool non_rigid) {
    if (non_rigid) {
        return transform_normal(rotation(a), b);
    } else {
        return normalize(transform_vector(a, b));
    }
}

// Translation, scaling and rotations transforms.
inline frame3f translation_frame(const vec3f &a) {
    return {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}, a};
}

inline frame3f scaling_frame(const vec3f &a) {
    return {{a.x, 0,   0},
            {0,   a.y, 0},
            {0,   0,   a.z},
            {0,   0,   0}};
}

inline frame3f rotation_frame(const vec3f &axis, float angle) {
    auto s = sin(angle), c = cos(angle);
    auto vv = normalize(axis);
    return {{c + (1 - c) * vv.x * vv.x,        (1 - c) * vv.x * vv.y + s * vv.z,
                                                  (1 - c) * vv.x * vv.z - s * vv.y},
            {(1 - c) * vv.x * vv.y - s * vv.z, c + (1 - c) * vv.y * vv.y,
                                                  (1 - c) * vv.y * vv.z + s * vv.x},
            {(1 - c) * vv.x * vv.z + s * vv.y, (1 - c) * vv.y * vv.z - s * vv.x,
                                                  c + (1 - c) * vv.z * vv.z},
            {0,                                0, 0}};
}

inline frame3f rotation_frame(const vec4f &quat) {
    auto v = quat;
    return {{v.w * v.w + v.x * v.x - v.y * v.y - v.z * v.z,
                                          (v.x * v.y + v.z * v.w) * 2, (v.z * v.x - v.y * v.w) * 2},
            {(v.x * v.y - v.z * v.w) * 2,
                                          v.w * v.w - v.x * v.x + v.y * v.y - v.z * v.z,
                                                                       (v.y * v.z + v.x * v.w) * 2},
            {(v.z * v.x + v.y * v.w) * 2, (v.y * v.z - v.x * v.w) * 2,
                                                                       v.w * v.w - v.x * v.x - v.y * v.y + v.z * v.z},
            {0,                           0,                           0}};
}

inline frame3f rotation_frame(const quat4f &quat) {
    auto v = quat;
    return {{v.w * v.w + v.x * v.x - v.y * v.y - v.z * v.z,
                                          (v.x * v.y + v.z * v.w) * 2, (v.z * v.x - v.y * v.w) * 2},
            {(v.x * v.y - v.z * v.w) * 2,
                                          v.w * v.w - v.x * v.x + v.y * v.y - v.z * v.z,
                                                                       (v.y * v.z + v.x * v.w) * 2},
            {(v.z * v.x + v.y * v.w) * 2, (v.y * v.z - v.x * v.w) * 2,
                                                                       v.w * v.w - v.x * v.x - v.y * v.y + v.z * v.z},
            {0,                           0,                           0}};
}

inline frame3f rotation_frame(const mat3f &rot) {
    return {rot.x, rot.y, rot.z, {0, 0, 0}};
}

// Lookat frame. Z-axis can be inverted with inv_xz.
inline frame3f lookat_frame(
        const vec3f &eye, const vec3f &center, const vec3f &up, bool inv_xz) {
    auto w = normalize(eye - center);
    auto u = normalize(cross(up, w));
    auto v = normalize(cross(w, u));
    if (inv_xz) {
        w = -w;
        u = -u;
    }
    return {u, v, w, eye};
}

// OpenGL frustum, ortho and perspecgive matrices.
inline mat4f frustum_mat(float l, float r, float b, float t, float n, float f) {
    return {{2 * n / (r - l),   0,                 0,                    0},
            {0,                 2 * n / (t - b),   0,                    0},
            {(r + l) / (r - l), (t + b) / (t - b), -(f + n) / (f - n),   -1},
            {0,                 0,                 -2 * f * n / (f - n), 0}};
}

inline mat4f ortho_mat(float l, float r, float b, float t, float n, float f) {
    return {{2 / (r - l),        0,                  0,                  0},
            {0,                  2 / (t - b),        0,                  0},
            {0,                  0,                  -2 / (f - n),       0},
            {-(r + l) / (r - l), -(t + b) / (t - b), -(f + n) / (f - n), 1}};
}

inline mat4f ortho2d_mat(float left, float right, float bottom, float top) {
    return ortho_mat(left, right, bottom, top, -1, 1);
}

inline mat4f ortho_mat(float xmag, float ymag, float near, float far) {
    return {{1 / xmag, 0,        0,                           0},
            {0,        1 / ymag, 0,                           0},
            {0,        0,        2 / (near - far),            0},
            {0,        0,        (far + near) / (near - far), 1}};
}

inline mat4f perspective_mat(float fovy, float aspect, float near, float far) {
    auto tg = tan(fovy / 2);
    return {{1 / (aspect * tg), 0,      0,                             0},
            {0,                 1 / tg, 0,                             0},
            {0,                 0,      (far + near) / (near - far),   -1},
            {0,                 0,      2 * far * near / (near - far), 0}};
}

inline mat4f perspective_mat(float fovy, float aspect, float near) {
    auto tg = tan(fovy / 2);
    return {{1 / (aspect * tg), 0,      0,        0},
            {0,                 1 / tg, 0,        0},
            {0,                 0,      -1,       -1},
            {0,                 0,      2 * near, 0}};
}

// Rotation conversions.
inline std::pair<vec3f, float> rotation_axisangle(const vec4f &quat) {
    return {normalize(vec3f{quat.x, quat.y, quat.z}), 2 * acos(quat.w)};
}

inline vec4f rotation_quat(const vec3f &axis, float angle) {
    auto len = length(axis);
    if (len == 0) return {0, 0, 0, 1};
    return vec4f{sin(angle / 2) * axis.x / len, sin(angle / 2) * axis.y / len,
                 sin(angle / 2) * axis.z / len, cos(angle / 2)};
}

inline vec4f rotation_quat(const vec4f &axisangle) {
    return rotation_quat(
            vec3f{axisangle.x, axisangle.y, axisangle.z}, axisangle.w);
}

}  // namespace bcg

#endif //BCG_GRAPHICS_BCG_TRANSFORM_H
