//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_QUATERNIONS_H
#define BCG_GRAPHICS_BCG_QUATERNIONS_H


// -----------------------------------------------------------------------------
// QUATERNIONS
// -----------------------------------------------------------------------------
namespace bcg {

// Quaternions to represent rotations
struct quat4f {
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1;
};

// Constants
inline const auto identity_quat4f = quat4f{0, 0, 0, 1};

// Quaternion operatons
inline quat4f operator+(const quat4f &a, const quat4f &b);

inline quat4f operator*(const quat4f &a, float b);

inline quat4f operator/(const quat4f &a, float b);

inline quat4f operator*(const quat4f &a, const quat4f &b);

// Quaterion operations
inline float dot(const quat4f &a, const quat4f &b);

inline float length(const quat4f &a);

inline quat4f normalize(const quat4f &a);

inline quat4f conjugate(const quat4f &a);

inline quat4f inverse(const quat4f &a);

inline float uangle(const quat4f &a, const quat4f &b);

inline quat4f lerp(const quat4f &a, const quat4f &b, float t);

inline quat4f nlerp(const quat4f &a, const quat4f &b, float t);

inline quat4f slerp(const quat4f &a, const quat4f &b, float t);

}

// -----------------------------------------------------------------------------
//
//
// IMPLEMENTATION
//
//
// -----------------------------------------------------------------------------

namespace bcg {

// Quaternion operatons
inline quat4f operator+(const quat4f &a, const quat4f &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

inline quat4f operator*(const quat4f &a, float b) {
    return {a.x * b, a.y * b, a.z * b, a.w * b};
}

inline quat4f operator/(const quat4f &a, float b) {
    return {a.x / b, a.y / b, a.z / b, a.w / b};
}

inline quat4f operator*(const quat4f &a, const quat4f &b) {
    return {a.x * b.w + a.w * b.x + a.y * b.w - a.z * b.y,
            a.y * b.w + a.w * b.y + a.z * b.x - a.x * b.z,
            a.z * b.w + a.w * b.z + a.x * b.y - a.y * b.x,
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z};
}

// Quaterion operations
inline float dot(const quat4f &a, const quat4f &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline float length(const quat4f &a) { return sqrt(dot(a, a)); }

inline quat4f normalize(const quat4f &a) {
    auto l = length(a);
    return (l != 0) ? a / l : a;
}

inline quat4f conjugate(const quat4f &a) { return {-a.x, -a.y, -a.z, a.w}; }

inline quat4f inverse(const quat4f &a) { return conjugate(a) / dot(a, a); }

inline float uangle(const quat4f &a, const quat4f &b) {
    auto d = dot(a, b);
    return d > 1 ? 0 : acos(d < -1 ? -1 : d);
}

inline quat4f lerp(const quat4f &a, const quat4f &b, float t) {
    return a * (1 - t) + b * t;
}

inline quat4f nlerp(const quat4f &a, const quat4f &b, float t) {
    return normalize(lerp(a, b, t));
}

inline quat4f slerp(const quat4f &a, const quat4f &b, float t) {
    auto th = uangle(a, b);
    return th == 0
           ? a
           : a * (sin(th * (1 - t)) / sin(th)) + b * (sin(th * t) / sin(th));
}

}  // namespace bcg

#endif //BCG_GRAPHICS_BCG_QUATERNIONS_H
