//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_FRAMES_H
#define BCG_GRAPHICS_BCG_FRAMES_H


// -----------------------------------------------------------------------------
// RIGID BODY TRANSFORMS/FRAMES
// -----------------------------------------------------------------------------
namespace bcg {

// Rigid frames stored as a column-major affine transform matrix.
struct frame2f {
    vec2f x = {1, 0};
    vec2f y = {0, 1};
    vec2f o = {0, 0};

    vec2f &operator[](int i);

    const vec2f &operator[](int i) const;
};

// Rigid frames stored as a column-major affine transform matrix.
struct frame3f {
    vec3f x = {1, 0, 0};
    vec3f y = {0, 1, 0};
    vec3f z = {0, 0, 1};
    vec3f o = {0, 0, 0};

    vec3f &operator[](int i);

    const vec3f &operator[](int i) const;
};

// Indentity frames.
inline const auto identity2x3f = frame2f{{1, 0},
                                         {0, 1},
                                         {0, 0}};
inline const auto identity3x4f = frame3f{
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1},
        {0, 0, 0}};

// Frame properties
inline mat2f rotation(const frame2f &a);

inline vec2f translation(const frame2f &a);

// Frame construction
inline frame2f make_frame(const mat2f &m, const vec2f &t);

// Conversion between frame and mat
inline mat3f frame_to_mat(const frame2f &f);

inline frame2f mat_to_frame(const mat3f &ma);

// Frame comparisons.
inline bool operator==(const frame2f &a, const frame2f &b);

inline bool operator!=(const frame2f &a, const frame2f &b);

// Frame composition, equivalent to affine matrix product.
inline frame2f operator*(const frame2f &a, const frame2f &b);

inline frame2f &operator*=(frame2f &a, const frame2f &b);

// Frame inverse, equivalent to rigid affine inverse.
inline frame2f inverse(const frame2f &a, bool non_rigid = false);

// Frame properties
inline mat3f rotation(const frame3f &a);

inline vec3f translation(const frame3f &a);

// Frame construction
inline frame3f make_frame(const mat3f &m, const vec3f &t);

// Conversion between frame and mat
inline mat4f frame_to_mat(const frame3f &f);

inline frame3f mat_to_frame(const mat4f &m);

// Frame comparisons.
inline bool operator==(const frame3f &a, const frame3f &b);

inline bool operator!=(const frame3f &a, const frame3f &b);

// Frame composition, equivalent to affine matrix product.
inline frame3f operator*(const frame3f &a, const frame3f &b);

inline frame3f &operator*=(frame3f &a, const frame3f &b);

// Frame inverse, equivalent to rigid affine inverse.
inline frame3f inverse(const frame3f &a, bool non_rigid = false);

// Frame construction from axis.
inline frame3f frame_fromz(const vec3f &o, const vec3f &v);

inline frame3f frame_fromzx(const vec3f &o, const vec3f &z_, const vec3f &x_);

}

// -----------------------------------------------------------------------------
//
//
// IMPLEMENTATION
//
//
// -----------------------------------------------------------------------------

namespace bcg {

// Rigid frames stored as a column-major affine transform matrix.
inline vec2f &frame2f::operator[](int i) { return (&x)[i]; }

inline const vec2f &frame2f::operator[](int i) const { return (&x)[i]; }

// Rigid frames stored as a column-major affine transform matrix.
inline vec3f &frame3f::operator[](int i) { return (&x)[i]; }

inline const vec3f &frame3f::operator[](int i) const { return (&x)[i]; }

// Frame properties
inline mat2f rotation(const frame2f &a) { return {a.x, a.y}; }

inline vec2f translation(const frame2f &a) { return a.o; }

// Frame construction
inline frame2f make_frame(const mat2f &m, const vec2f &t) {
    return {m.x, m.y, t};
}

// Frame/mat conversion
inline frame2f mat_to_frame(const mat3f &m) {
    return {{m.x.x, m.x.y},
            {m.y.x, m.y.y},
            {m.z.x, m.z.y}};
}

inline mat3f frame_to_mat(const frame2f &f) {
    return {{f.x.x, f.x.y, 0},
            {f.y.x, f.y.y, 0},
            {f.o.x, f.o.y, 1}};
}

// Frame comparisons.
inline bool operator==(const frame2f &a, const frame2f &b) {
    return a.x == b.x && a.y == b.y && a.o == b.o;
}

inline bool operator!=(const frame2f &a, const frame2f &b) { return !(a == b); }

// Frame composition, equivalent to affine matrix product.
inline frame2f operator*(const frame2f &a, const frame2f &b) {
    return make_frame(rotation(a) * rotation(b), rotation(a) * b.o + a.o);
}

inline frame2f &operator*=(frame2f &a, const frame2f &b) { return a = a * b; }

// Frame inverse, equivalent to rigid affine inverse.
inline frame2f inverse(const frame2f &a, bool non_rigid) {
    if (non_rigid) {
        auto minv = inverse(rotation(a));
        return make_frame(minv, -(minv * a.o));
    } else {
        auto minv = transpose(rotation(a));
        return make_frame(minv, -(minv * a.o));
    }
}

// Frame properties
inline mat3f rotation(const frame3f &a) { return {a.x, a.y, a.z}; }

inline vec3f translation(const frame3f &a) { return a.o; }

// Frame construction
inline frame3f make_frame(const mat3f &m, const vec3f &t) {
    return {m.x, m.y, m.z, t};
}

// frame/mat conversion
inline frame3f mat_to_frame(const mat4f &m) {
    return {{m.x.x, m.x.y, m.x.z},
            {m.y.x, m.y.y, m.y.z},
            {m.z.x, m.z.y, m.z.z},
            {m.w.x, m.w.y, m.w.z}};
}

inline mat4f frame_to_mat(const frame3f &f) {
    return {{f.x.x, f.x.y, f.x.z, 0},
            {f.y.x, f.y.y, f.y.z, 0},
            {f.z.x, f.z.y, f.z.z, 0},
            {f.o.x, f.o.y, f.o.z, 1}};
}

// Frame comparisons.
inline bool operator==(const frame3f &a, const frame3f &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z && a.o == b.o;
}

inline bool operator!=(const frame3f &a, const frame3f &b) { return !(a == b); }

// Frame composition, equivalent to affine matrix product.
inline frame3f operator*(const frame3f &a, const frame3f &b) {
    return make_frame(rotation(a) * rotation(b), rotation(a) * b.o + a.o);
}

inline frame3f &operator*=(frame3f &a, const frame3f &b) { return a = a * b; }

// Frame inverse, equivalent to rigid affine inverse.
inline frame3f inverse(const frame3f &a, bool non_rigid) {
    if (non_rigid) {
        auto minv = inverse(rotation(a));
        return make_frame(minv, -(minv * a.o));
    } else {
        auto minv = transpose(rotation(a));
        return make_frame(minv, -(minv * a.o));
    }
}

// Frame construction from axis.
inline frame3f frame_fromz(const vec3f &o, const vec3f &v) {
    // https://graphics.pixar.com/library/OrthonormalB/paper.pdf
    auto z = normalize(v);
    auto sign = copysignf(1.0f, z.z);
    auto a = -1.0f / (sign + z.z);
    auto b = z.x * z.y * a;
    auto x = vec3f{1.0f + sign * z.x * z.x * a, sign * b, -sign * z.x};
    auto y = vec3f{b, sign + z.y * z.y * a, -z.y};
    return {x, y, z, o};
}

inline frame3f frame_fromzx(const vec3f &o, const vec3f &z_, const vec3f &x_) {
    auto z = normalize(z_);
    auto x = orthonormalize(x_, z);
    auto y = normalize(cross(z, x));
    return {x, y, z, o};
}

}  // namespace bcg

#endif //BCG_GRAPHICS_BCG_FRAMES_H
