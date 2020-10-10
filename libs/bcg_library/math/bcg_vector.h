//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_VECTOR_H
#define BCG_GRAPHICS_BCG_VECTOR_H

#include "bcg_math_constant.h"

// -----------------------------------------------------------------------------
// VECTORS
// -----------------------------------------------------------------------------
namespace bcg {

struct vec2f {
    float x = 0;
    float y = 0;

    float &operator[](int i);

    const float &operator[](int i) const;
};

struct vec3f {
    float x = 0;
    float y = 0;
    float z = 0;

    float &operator[](int i);

    const float &operator[](int i) const;
};

struct vec4f {
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 0;

    float &operator[](int i);

    const float &operator[](int i) const;

};

// Element access
inline vec3f xyz(const vec4f &a);

// Zero vector constants.
inline const auto zero2f = vec2f{0, 0};
inline const auto zero3f = vec3f{0, 0, 0};
inline const auto zero4f = vec4f{0, 0, 0, 0};

// One vector constants.
inline const auto one2f = vec2f{1, 1};
inline const auto one3f = vec3f{1, 1, 1};
inline const auto one4f = vec4f{1, 1, 1, 1};

inline int size(const vec2f &a);

inline const float *begin(vec2f a);

inline const float *end(const vec2f &a);

inline float *begin(vec2f &a);

inline float *end(vec2f &a);

inline const float *data(const vec2f &a);

inline float *data(vec2f &a);

// Vector comparison operations.
inline bool operator==(const vec2f &a, const vec2f &b);

inline bool operator!=(const vec2f &a, const vec2f &b);

// Vector operations.
inline vec2f operator+(const vec2f &a);

inline vec2f operator-(const vec2f &a);

inline vec2f operator+(const vec2f &a, const vec2f &b);

inline vec2f operator+(const vec2f &a, float b);

inline vec2f operator+(float a, const vec2f &b);

inline vec2f operator-(const vec2f &a, const vec2f &b);

inline vec2f operator-(const vec2f &a, float b);

inline vec2f operator-(float a, const vec2f &b);

inline vec2f operator*(const vec2f &a, const vec2f &b);

inline vec2f operator*(const vec2f &a, float b);

inline vec2f operator*(float a, const vec2f &b);

inline vec2f operator/(const vec2f &a, const vec2f &b);

inline vec2f operator/(const vec2f &a, float b);

inline vec2f operator/(float a, const vec2f &b);

// Vector assignments
inline vec2f &operator+=(vec2f &a, const vec2f &b);

inline vec2f &operator+=(vec2f &a, float b);

inline vec2f &operator-=(vec2f &a, const vec2f &b);

inline vec2f &operator-=(vec2f &a, float b);

inline vec2f &operator*=(vec2f &a, const vec2f &b);

inline vec2f &operator*=(vec2f &a, float b);

inline vec2f &operator/=(vec2f &a, const vec2f &b);

inline vec2f &operator/=(vec2f &a, float b);

// Vector products and lengths.
inline float dot(const vec2f &a, const vec2f &b);

inline float cross(const vec2f &a, const vec2f &b);

inline float length(const vec2f &a);

inline float length_squared(const vec2f &a);

inline vec2f normalize(const vec2f &a);

inline float distance(const vec2f &a, const vec2f &b);

inline float distance_squared(const vec2f &a, const vec2f &b);

inline float angle(const vec2f &a, const vec2f &b);

// Max element and clamp.
inline vec2f max(const vec2f &a, float b);

inline vec2f min(const vec2f &a, float b);

inline vec2f max(const vec2f &a, const vec2f &b);

inline vec2f min(const vec2f &a, const vec2f &b);

inline vec2f clamp(const vec2f &x, float min, float max);

inline vec2f lerp(const vec2f &a, const vec2f &b, float u);

inline vec2f lerp(const vec2f &a, const vec2f &b, const vec2f &u);

inline float max(const vec2f &a);

inline float min(const vec2f &a);

inline float sum(const vec2f &a);

inline float mean(const vec2f &a);

// Functions applied to vector elements
inline vec2f abs(const vec2f &a);

inline vec2f sqrt(const vec2f &a);

inline vec2f exp(const vec2f &a);

inline vec2f log(const vec2f &a);

inline vec2f exp2(const vec2f &a);

inline vec2f log2(const vec2f &a);

inline bool isfinite(const vec2f &a);

inline vec2f pow(const vec2f &a, float b);

inline vec2f pow(const vec2f &a, const vec2f &b);

inline vec2f gain(const vec2f &a, float b);

inline void swap(vec2f &a, vec2f &b);

// Vector sequence operations.
inline int size(const vec3f &a);

inline const float *begin(const vec3f &a);

inline const float *end(const vec3f &a);

inline float *begin(vec3f &a);

inline float *end(vec3f &a);

inline const float *data(const vec3f &a);

inline float *data(vec3f &a);

// Vector comparison operations.
inline bool operator==(const vec3f &a, const vec3f &b);

inline bool operator!=(const vec3f &a, const vec3f &b);

// Vector operations.
inline vec3f operator+(const vec3f &a);

inline vec3f operator-(const vec3f &a);

inline vec3f operator+(const vec3f &a, const vec3f &b);

inline vec3f operator+(const vec3f &a, float b);

inline vec3f operator+(float a, const vec3f &b);

inline vec3f operator-(const vec3f &a, const vec3f &b);

inline vec3f operator-(const vec3f &a, float b);

inline vec3f operator-(float a, const vec3f &b);

inline vec3f operator*(const vec3f &a, const vec3f &b);

inline vec3f operator*(const vec3f &a, float b);

inline vec3f operator*(float a, const vec3f &b);

inline vec3f operator/(const vec3f &a, const vec3f &b);

inline vec3f operator/(const vec3f &a, float b);

inline vec3f operator/(float a, const vec3f &b);

// Vector assignments
inline vec3f &operator+=(vec3f &a, const vec3f &b);

inline vec3f &operator+=(vec3f &a, float b);

inline vec3f &operator-=(vec3f &a, const vec3f &b);

inline vec3f &operator-=(vec3f &a, float b);

inline vec3f &operator*=(vec3f &a, const vec3f &b);

inline vec3f &operator*=(vec3f &a, float b);

inline vec3f &operator/=(vec3f &a, const vec3f &b);

inline vec3f &operator/=(vec3f &a, float b);


// Vector products and lengths.
inline float dot(const vec3f &a, const vec3f &b);

inline vec3f cross(const vec3f &a, const vec3f &b);

inline float length(const vec3f &a);

inline float length_squared(const vec3f &a);

inline vec3f normalize(const vec3f &a);

inline float distance(const vec3f &a, const vec3f &b);

inline float distance_squared(const vec3f &a, const vec3f &b);

inline float angle(const vec3f &a, const vec3f &b);

// Orthogonal vectors.
inline vec3f orthogonal(const vec3f &v);

inline vec3f orthonormalize(const vec3f &a, const vec3f &b);

// Reflected and refracted vector.
inline vec3f reflect(const vec3f &w, const vec3f &n);

inline vec3f refract(const vec3f &w, const vec3f &n, float inv_eta);

// Max element and clamp.
inline vec3f max(const vec3f &a, float b);

inline vec3f min(const vec3f &a, float b);

inline vec3f max(const vec3f &a, const vec3f &b);

inline vec3f min(const vec3f &a, const vec3f &b);

inline vec3f clamp(const vec3f &x, float min, float max);

inline vec3f lerp(const vec3f &a, const vec3f &b, float u);

inline vec3f lerp(const vec3f &a, const vec3f &b, const vec3f &u);

inline float max(const vec3f &a);

inline float min(const vec3f &a);

inline float sum(const vec3f &a);

inline float mean(const vec3f &a);

// Functions applied to vector elements
inline vec3f abs(const vec3f &a);

inline vec3f sqrt(const vec3f &a);

inline vec3f exp(const vec3f &a);

inline vec3f log(const vec3f &a);

inline vec3f exp2(const vec3f &a);

inline vec3f log2(const vec3f &a);

inline vec3f pow(const vec3f &a, float b);

inline vec3f pow(const vec3f &a, const vec3f &b);

inline vec3f gain(const vec3f &a, float b);

inline bool isfinite(const vec3f &a);

inline void swap(vec3f &a, vec3f &b);

// Vector sequence operations.
inline int size(const vec4f &a);

inline const float *begin(const vec4f &a);

inline const float *end(const vec4f &a);

inline float *begin(vec4f &a);

inline float *end(vec4f &a);

inline const float *data(const vec4f &a);

inline float *data(vec4f &a);

// Vector comparison operations.
inline bool operator==(const vec4f &a, const vec4f &b);

inline bool operator!=(const vec4f &a, const vec4f &b);

// Vector operations.
inline vec4f operator+(const vec4f &a);

inline vec4f operator-(const vec4f &a);

inline vec4f operator+(const vec4f &a, const vec4f &b);

inline vec4f operator+(const vec4f &a, float b);

inline vec4f operator+(float a, const vec4f &b);

inline vec4f operator-(const vec4f &a, const vec4f &b);

inline vec4f operator-(const vec4f &a, float b);

inline vec4f operator-(float a, const vec4f &b);

inline vec4f operator*(const vec4f &a, const vec4f &b);

inline vec4f operator*(const vec4f &a, float b);

inline vec4f operator*(float a, const vec4f &b);

inline vec4f operator/(const vec4f &a, const vec4f &b);

inline vec4f operator/(const vec4f &a, float b);

inline vec4f operator/(float a, const vec4f &b);

// Vector assignments
inline vec4f &operator+=(vec4f &a, const vec4f &b);

inline vec4f &operator+=(vec4f &a, float b);

inline vec4f &operator-=(vec4f &a, const vec4f &b);

inline vec4f &operator-=(vec4f &a, float b);

inline vec4f &operator*=(vec4f &a, const vec4f &b);

inline vec4f &operator*=(vec4f &a, float b);

inline vec4f &operator/=(vec4f &a, const vec4f &b);

inline vec4f &operator/=(vec4f &a, float b);

// Vector products and lengths.
inline float dot(const vec4f &a, const vec4f &b);

inline float length(const vec4f &a);

inline float length_squared(const vec4f &a);

inline vec4f normalize(const vec4f &a);

inline float distance(const vec4f &a, const vec4f &b);

inline float distance_squared(const vec4f &a, const vec4f &b);

inline float angle(const vec4f &a, const vec4f &b);

inline vec4f slerp(const vec4f &a, const vec4f &b, float u);

// Max element and clamp.
inline vec4f max(const vec4f &a, float b);

inline vec4f min(const vec4f &a, float b);

inline vec4f max(const vec4f &a, const vec4f &b);

inline vec4f min(const vec4f &a, const vec4f &b);

inline vec4f clamp(const vec4f &x, float min, float max);

inline vec4f lerp(const vec4f &a, const vec4f &b, float u);

inline vec4f lerp(const vec4f &a, const vec4f &b, const vec4f &u);

inline float max(const vec4f &a);

inline float min(const vec4f &a);

inline float sum(const vec4f &a);

inline float mean(const vec4f &a);

// Functions applied to vector elements
inline vec4f abs(const vec4f &a);

inline vec4f sqrt(const vec4f &a);

inline vec4f exp(const vec4f &a);

inline vec4f log(const vec4f &a);

inline vec4f exp2(const vec4f &a);

inline vec4f log2(const vec4f &a);

inline vec4f pow(const vec4f &a, float b);

inline vec4f pow(const vec4f &a, const vec4f &b);

inline vec4f gain(const vec4f &a, float b);

inline bool isfinite(const vec4f &a);

inline void swap(vec4f &a, vec4f &b);

// Quaternion operatons represented as xi + yj + zk + w
// const auto identity_quat4f = vec4f{0, 0, 0, 1};
inline vec4f quat_mul(const vec4f &a, float b);

inline vec4f quat_mul(const vec4f &a, const vec4f &b);

inline vec4f quat_conjugate(const vec4f &a);

inline vec4f quat_inverse(const vec4f &a);
}

// -----------------------------------------------------------------------------
// INTEGER VECTORS
// -----------------------------------------------------------------------------
namespace bcg {

struct vec2i {
    int x = 0;
    int y = 0;

    int &operator[](int i);

    const int &operator[](int i) const;
};

struct vec3i {
    int x = 0;
    int y = 0;
    int z = 0;

    int &operator[](int i);

    const int &operator[](int i) const;
};

struct vec4i {
    int x = 0;
    int y = 0;
    int z = 0;
    int w = 0;

    int &operator[](int i);

    const int &operator[](int i) const;
};

struct vec3b {
    byte x = 0;
    byte y = 0;
    byte z = 0;

    byte &operator[](int i);

    const byte &operator[](int i) const;
};

struct vec4b {
    byte x = 0;
    byte y = 0;
    byte z = 0;
    byte w = 0;

    byte &operator[](int i);

    const byte &operator[](int i) const;
};

struct vec3s {
    ushort x = 0;
    ushort y = 0;
    ushort z = 0;

    ushort &operator[](int i);

    const ushort &operator[](int i) const;
};

struct vec4s {
    ushort x = 0;
    ushort y = 0;
    ushort z = 0;
    ushort w = 0;

    ushort &operator[](int i);

    const ushort &operator[](int i) const;
};

// Zero vector constants.
inline const auto zero2i = vec2i{0, 0};
inline const auto zero3i = vec3i{0, 0, 0};
inline const auto zero4i = vec4i{0, 0, 0, 0};
inline const auto zero3b = vec3b{0, 0, 0};
inline const auto zero4b = vec4b{0, 0, 0, 0};
inline const auto zero3s = vec3s{0, 0, 0};
inline const auto zero4s = vec4s{0, 0, 0, 0};

// Element access
inline vec3i xyz(const vec4i &a);

// Element access
inline vec3b xyz(const vec4b &a);

// Element access
inline vec3s xyz(const vec4s &a);

// Vector sequence operations.
inline int size(const vec2i &a);

inline const int *begin(const vec2i &a);

inline const int *end(const vec2i &a);

inline int *begin(vec2i &a);

inline int *end(vec2i &a);

inline const int *data(const vec2i &a);

inline int *data(vec2i &a);

// Vector comparison operations.
inline bool operator==(const vec2i &a, const vec2i &b);

inline bool operator!=(const vec2i &a, const vec2i &b);

// Vector operations.
inline vec2i operator+(const vec2i &a);

inline vec2i operator-(const vec2i &a);

inline vec2i operator+(const vec2i &a, const vec2i &b);

inline vec2i operator+(const vec2i &a, int b);

inline vec2i operator+(int a, const vec2i &b);

inline vec2i operator-(const vec2i &a, const vec2i &b);

inline vec2i operator-(const vec2i &a, int b);

inline vec2i operator-(int a, const vec2i &b);

inline vec2i operator*(const vec2i &a, const vec2i &b);

inline vec2i operator*(const vec2i &a, int b);

inline vec2i operator*(int a, const vec2i &b);

inline vec2i operator/(const vec2i &a, const vec2i &b);

inline vec2i operator/(const vec2i &a, int b);

inline vec2i operator/(int a, const vec2i &b);

// Vector assignments
inline vec2i &operator+=(vec2i &a, const vec2i &b);

inline vec2i &operator+=(vec2i &a, int b);

inline vec2i &operator-=(vec2i &a, const vec2i &b);

inline vec2i &operator-=(vec2i &a, int b);

inline vec2i &operator*=(vec2i &a, const vec2i &b);

inline vec2i &operator*=(vec2i &a, int b);

inline vec2i &operator/=(vec2i &a, const vec2i &b);

inline vec2i &operator/=(vec2i &a, int b);

// Max element and clamp.
inline vec2i max(const vec2i &a, int b);

inline vec2i min(const vec2i &a, int b);

inline vec2i max(const vec2i &a, const vec2i &b);

inline vec2i min(const vec2i &a, const vec2i &b);

inline vec2i clamp(const vec2i &x, int min, int max);

inline int max(const vec2i &a);

inline int min(const vec2i &a);

inline int sum(const vec2i &a);

// Functions applied to vector elements
inline vec2i abs(const vec2i &a);

inline void swap(vec2i &a, vec2i &b);

// Vector sequence operations.
inline int size(const vec3i &a);

inline const int *begin(const vec3i &a);

inline const int *end(const vec3i &a);

inline int *begin(vec3i &a);

inline int *end(vec3i &a);

inline const int *data(const vec3i &a);

inline int *data(vec3i &a);

// Vector comparison operations.
inline bool operator==(const vec3i &a, const vec3i &b);

inline bool operator!=(const vec3i &a, const vec3i &b);

// Vector operations.
inline vec3i operator+(const vec3i &a);

inline vec3i operator-(const vec3i &a);

inline vec3i operator+(const vec3i &a, const vec3i &b);

inline vec3i operator+(const vec3i &a, int b);

inline vec3i operator+(int a, const vec3i &b);

inline vec3i operator-(const vec3i &a, const vec3i &b);

inline vec3i operator-(const vec3i &a, int b);

inline vec3i operator-(int a, const vec3i &b);

inline vec3i operator*(const vec3i &a, const vec3i &b);

inline vec3i operator*(const vec3i &a, int b);

inline vec3i operator*(int a, const vec3i &b);

inline vec3i operator/(const vec3i &a, const vec3i &b);

inline vec3i operator/(const vec3i &a, int b);

inline vec3i operator/(int a, const vec3i &b);

// Vector assignments
inline vec3i &operator+=(vec3i &a, const vec3i &b);

inline vec3i &operator+=(vec3i &a, int b);

inline vec3i &operator-=(vec3i &a, const vec3i &b);

inline vec3i &operator-=(vec3i &a, int b);

inline vec3i &operator*=(vec3i &a, const vec3i &b);

inline vec3i &operator*=(vec3i &a, int b);

inline vec3i &operator/=(vec3i &a, const vec3i &b);

inline vec3i &operator/=(vec3i &a, int b);

// Max element and clamp.
inline vec3i max(const vec3i &a, int b);

inline vec3i min(const vec3i &a, int b);

inline vec3i max(const vec3i &a, const vec3i &b);

inline vec3i min(const vec3i &a, const vec3i &b);

inline vec3i clamp(const vec3i &x, int min, int max);

inline int max(const vec3i &a);

inline int min(const vec3i &a);

inline int sum(const vec3i &a);

// Functions applied to vector elements
inline vec3i abs(const vec3i &a);

inline void swap(vec3i &a, vec3i &b);

// Vector sequence operations.
inline int size(const vec4i &a);

inline const int *begin(const vec4i &a);

inline const int *end(const vec4i &a);

inline int *begin(vec4i &a);

inline int *end(vec4i &a);

inline const int *data(const vec4i &a);

inline int *data(vec4i &a);

// Vector comparison operations.
inline bool operator==(const vec4i &a, const vec4i &b);

inline bool operator!=(const vec4i &a, const vec4i &b);

// Vector operations.
inline vec4i operator+(const vec4i &a);

inline vec4i operator-(const vec4i &a);

inline vec4i operator+(const vec4i &a, const vec4i &b);

inline vec4i operator+(const vec4i &a, int b);

inline vec4i operator+(int a, const vec4i &b);

inline vec4i operator-(const vec4i &a, const vec4i &b);

inline vec4i operator-(const vec4i &a, int b);

inline vec4i operator-(int a, const vec4i &b);

inline vec4i operator*(const vec4i &a, const vec4i &b);

inline vec4i operator*(const vec4i &a, int b);

inline vec4i operator*(int a, const vec4i &b);

inline vec4i operator/(const vec4i &a, const vec4i &b);

inline vec4i operator/(const vec4i &a, int b);

inline vec4i operator/(int a, const vec4i &b);

// Vector assignments
inline vec4i &operator+=(vec4i &a, const vec4i &b);

inline vec4i &operator+=(vec4i &a, int b);

inline vec4i &operator-=(vec4i &a, const vec4i &b);

inline vec4i &operator-=(vec4i &a, int b);

inline vec4i &operator*=(vec4i &a, const vec4i &b);

inline vec4i &operator*=(vec4i &a, int b);

inline vec4i &operator/=(vec4i &a, const vec4i &b);

inline vec4i &operator/=(vec4i &a, int b);

// Max element and clamp.
inline vec4i max(const vec4i &a, int b);

inline vec4i min(const vec4i &a, int b);

inline vec4i max(const vec4i &a, const vec4i &b);

inline vec4i min(const vec4i &a, const vec4i &b);

inline vec4i clamp(const vec4i &x, int min, int max);

inline int max(const vec4i &a);

inline int min(const vec4i &a);

inline int sum(const vec4i &a);

// Functions applied to vector elements
inline vec4i abs(const vec4i &a);

inline void swap(vec4i &a, vec4i &b);
}


// -----------------------------------------------------------------------------
//
//
// IMPLEMENTATION
//
//
// -----------------------------------------------------------------------------

namespace bcg {

// Vec2
inline float &vec2f::operator[](int i) { return (&x)[i]; }

inline const float &vec2f::operator[](int i) const { return (&x)[i]; }

// Vec3
inline float &vec3f::operator[](int i) { return (&x)[i]; }

inline const float &vec3f::operator[](int i) const { return (&x)[i]; }

// Vec4
inline float &vec4f::operator[](int i) { return (&x)[i]; }

inline const float &vec4f::operator[](int i) const { return (&x)[i]; }

// Element access
inline vec3f xyz(const vec4f &a) { return {a.x, a.y, a.z}; }

// Vector sequence operations.
inline int size(const vec2f &a) { return 2; }

inline const float *begin(const vec2f &a) { return &a.x; }

inline const float *end(const vec2f &a) { return &a.x + 2; }

inline float *begin(vec2f &a) { return &a.x; }

inline float *end(vec2f &a) { return &a.x + 2; }

inline const float *data(const vec2f &a) { return &a.x; }

inline float *data(vec2f &a) { return &a.x; }

// Vector comparison operations.
inline bool operator==(const vec2f &a, const vec2f &b) {
    return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const vec2f &a, const vec2f &b) {
    return a.x != b.x || a.y != b.y;
}

// Vector operations.
inline vec2f operator+(const vec2f &a) { return a; }

inline vec2f operator-(const vec2f &a) { return {-a.x, -a.y}; }

inline vec2f operator+(const vec2f &a, const vec2f &b) {
    return {a.x + b.x, a.y + b.y};
}

inline vec2f operator+(const vec2f &a, float b) { return {a.x + b, a.y + b}; }

inline vec2f operator+(float a, const vec2f &b) { return {a + b.x, a + b.y}; }

inline vec2f operator-(const vec2f &a, const vec2f &b) {
    return {a.x - b.x, a.y - b.y};
}

inline vec2f operator-(const vec2f &a, float b) { return {a.x - b, a.y - b}; }

inline vec2f operator-(float a, const vec2f &b) { return {a - b.x, a - b.y}; }

inline vec2f operator*(const vec2f &a, const vec2f &b) {
    return {a.x * b.x, a.y * b.y};
}

inline vec2f operator*(const vec2f &a, float b) { return {a.x * b, a.y * b}; }

inline vec2f operator*(float a, const vec2f &b) { return {a * b.x, a * b.y}; }

inline vec2f operator/(const vec2f &a, const vec2f &b) {
    return {a.x / b.x, a.y / b.y};
}

inline vec2f operator/(const vec2f &a, float b) { return {a.x / b, a.y / b}; }

inline vec2f operator/(float a, const vec2f &b) { return {a / b.x, a / b.y}; }

// Vector assignments
inline vec2f &operator+=(vec2f &a, const vec2f &b) { return a = a + b; }

inline vec2f &operator+=(vec2f &a, float b) { return a = a + b; }

inline vec2f &operator-=(vec2f &a, const vec2f &b) { return a = a - b; }

inline vec2f &operator-=(vec2f &a, float b) { return a = a - b; }

inline vec2f &operator*=(vec2f &a, const vec2f &b) { return a = a * b; }

inline vec2f &operator*=(vec2f &a, float b) { return a = a * b; }

inline vec2f &operator/=(vec2f &a, const vec2f &b) { return a = a / b; }

inline vec2f &operator/=(vec2f &a, float b) { return a = a / b; }

// Vector products and lengths.
inline float dot(const vec2f &a, const vec2f &b) {
    return a.x * b.x + a.y * b.y;
}

inline float cross(const vec2f &a, const vec2f &b) {
    return a.x * b.y - a.y * b.x;
}

inline float length(const vec2f &a) { return sqrt(dot(a, a)); }

inline float length_squared(const vec2f &a) { return dot(a, a); }

inline vec2f normalize(const vec2f &a) {
    auto l = length(a);
    return (l != 0) ? a / l : a;
}

inline float distance(const vec2f &a, const vec2f &b) { return length(a - b); }

inline float distance_squared(const vec2f &a, const vec2f &b) {
    return dot(a - b, a - b);
}

inline float angle(const vec2f &a, const vec2f &b) {
    return acos(clamp(dot(normalize(a), normalize(b)), (float) -1, (float) 1));
}

// Max element and clamp.
inline vec2f max(const vec2f &a, float b) { return {max(a.x, b), max(a.y, b)}; }

inline vec2f min(const vec2f &a, float b) { return {min(a.x, b), min(a.y, b)}; }

inline vec2f max(const vec2f &a, const vec2f &b) {
    return {max(a.x, b.x), max(a.y, b.y)};
}

inline vec2f min(const vec2f &a, const vec2f &b) {
    return {min(a.x, b.x), min(a.y, b.y)};
}

inline vec2f clamp(const vec2f &x, float min, float max) {
    return {clamp(x.x, min, max), clamp(x.y, min, max)};
}

inline vec2f lerp(const vec2f &a, const vec2f &b, float u) {
    return a * (1 - u) + b * u;
}

inline vec2f lerp(const vec2f &a, const vec2f &b, const vec2f &u) {
    return a * (1 - u) + b * u;
}

inline float max(const vec2f &a) { return max(a.x, a.y); }

inline float min(const vec2f &a) { return min(a.x, a.y); }

inline float sum(const vec2f &a) { return a.x + a.y; }

inline float mean(const vec2f &a) { return sum(a) / 2; }

// Functions applied to vector elements
inline vec2f abs(const vec2f &a) { return {abs(a.x), abs(a.y)}; }

inline vec2f sqrt(const vec2f &a) { return {sqrt(a.x), sqrt(a.y)}; }

inline vec2f exp(const vec2f &a) { return {exp(a.x), exp(a.y)}; }

inline vec2f log(const vec2f &a) { return {log(a.x), log(a.y)}; }

inline vec2f exp2(const vec2f &a) { return {exp2(a.x), exp2(a.y)}; }

inline vec2f log2(const vec2f &a) { return {log2(a.x), log2(a.y)}; }

inline bool isfinite(const vec2f &a) { return isfinite(a.x) && isfinite(a.y); }

inline vec2f pow(const vec2f &a, float b) { return {pow(a.x, b), pow(a.y, b)}; }

inline vec2f pow(const vec2f &a, const vec2f &b) {
    return {pow(a.x, b.x), pow(a.y, b.y)};
}

inline vec2f gain(const vec2f &a, float b) {
    return {gain(a.x, b), gain(a.y, b)};
}

inline void swap(vec2f &a, vec2f &b) { std::swap(a, b); }

// Vector sequence operations.
inline int size(const vec3f &a) { return 3; }

inline const float *begin(const vec3f &a) { return &a.x; }

inline const float *end(const vec3f &a) { return &a.x + 3; }

inline float *begin(vec3f &a) { return &a.x; }

inline float *end(vec3f &a) { return &a.x + 3; }

inline const float *data(const vec3f &a) { return &a.x; }

inline float *data(vec3f &a) { return &a.x; }

// Vector comparison operations.
inline bool operator==(const vec3f &a, const vec3f &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

inline bool operator!=(const vec3f &a, const vec3f &b) {
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

// Vector operations.
inline vec3f operator+(const vec3f &a) { return a; }

inline vec3f operator-(const vec3f &a) { return {-a.x, -a.y, -a.z}; }

inline vec3f operator+(const vec3f &a, const vec3f &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

inline vec3f operator+(const vec3f &a, float b) {
    return {a.x + b, a.y + b, a.z + b};
}

inline vec3f operator+(float a, const vec3f &b) {
    return {a + b.x, a + b.y, a + b.z};
}

inline vec3f operator-(const vec3f &a, const vec3f &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

inline vec3f operator-(const vec3f &a, float b) {
    return {a.x - b, a.y - b, a.z - b};
}

inline vec3f operator-(float a, const vec3f &b) {
    return {a - b.x, a - b.y, a - b.z};
}

inline vec3f operator*(const vec3f &a, const vec3f &b) {
    return {a.x * b.x, a.y * b.y, a.z * b.z};
}

inline vec3f operator*(const vec3f &a, float b) {
    return {a.x * b, a.y * b, a.z * b};
}

inline vec3f operator*(float a, const vec3f &b) {
    return {a * b.x, a * b.y, a * b.z};
}

inline vec3f operator/(const vec3f &a, const vec3f &b) {
    return {a.x / b.x, a.y / b.y, a.z / b.z};
}

inline vec3f operator/(const vec3f &a, float b) {
    return {a.x / b, a.y / b, a.z / b};
}

inline vec3f operator/(float a, const vec3f &b) {
    return {a / b.x, a / b.y, a / b.z};
}

// Vector assignments
inline vec3f &operator+=(vec3f &a, const vec3f &b) { return a = a + b; }

inline vec3f &operator+=(vec3f &a, float b) { return a = a + b; }

inline vec3f &operator-=(vec3f &a, const vec3f &b) { return a = a - b; }

inline vec3f &operator-=(vec3f &a, float b) { return a = a - b; }

inline vec3f &operator*=(vec3f &a, const vec3f &b) { return a = a * b; }

inline vec3f &operator*=(vec3f &a, float b) { return a = a * b; }

inline vec3f &operator/=(vec3f &a, const vec3f &b) { return a = a / b; }

inline vec3f &operator/=(vec3f &a, float b) { return a = a / b; }

// Vector products and lengths.
inline float dot(const vec3f &a, const vec3f &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline vec3f cross(const vec3f &a, const vec3f &b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

inline float length(const vec3f &a) { return sqrt(dot(a, a)); }

inline float length_squared(const vec3f &a) { return dot(a, a); }

inline vec3f normalize(const vec3f &a) {
    auto l = length(a);
    return (l != 0) ? a / l : a;
}

inline float distance(const vec3f &a, const vec3f &b) { return length(a - b); }

inline float distance_squared(const vec3f &a, const vec3f &b) {
    return dot(a - b, a - b);
}

inline float angle(const vec3f &a, const vec3f &b) {
    return acos(clamp(dot(normalize(a), normalize(b)), (float) -1, (float) 1));
}

// Orthogonal vectors.
inline vec3f orthogonal(const vec3f &v) {
    // http://lolengine.net/blog/2013/09/21/picking-orthogonal-vector-combing-coconuts)
    return abs(v.x) > abs(v.z) ? vec3f{-v.y, v.x, 0} : vec3f{0, -v.z, v.y};
}

inline vec3f orthonormalize(const vec3f &a, const vec3f &b) {
    return normalize(a - b * dot(a, b));
}

// Reflected and refracted vector.
inline vec3f reflect(const vec3f &w, const vec3f &n) {
    return -w + 2 * dot(n, w) * n;
}

inline vec3f refract(const vec3f &w, const vec3f &n, float inv_eta) {
    auto cosine = dot(n, w);
    auto k = 1 + inv_eta * inv_eta * (cosine * cosine - 1);
    if (k < 0) return {0, 0, 0};  // tir
    return -w * inv_eta + (inv_eta * cosine - sqrt(k)) * n;
}

// Max element and clamp.
inline vec3f max(const vec3f &a, float b) {
    return {max(a.x, b), max(a.y, b), max(a.z, b)};
}

inline vec3f min(const vec3f &a, float b) {
    return {min(a.x, b), min(a.y, b), min(a.z, b)};
}

inline vec3f max(const vec3f &a, const vec3f &b) {
    return {max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)};
}

inline vec3f min(const vec3f &a, const vec3f &b) {
    return {min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)};
}

inline vec3f clamp(const vec3f &x, float min, float max) {
    return {clamp(x.x, min, max), clamp(x.y, min, max), clamp(x.z, min, max)};
}

inline vec3f lerp(const vec3f &a, const vec3f &b, float u) {
    return a * (1 - u) + b * u;
}

inline vec3f lerp(const vec3f &a, const vec3f &b, const vec3f &u) {
    return a * (1 - u) + b * u;
}

inline float max(const vec3f &a) { return max(max(a.x, a.y), a.z); }

inline float min(const vec3f &a) { return min(min(a.x, a.y), a.z); }

inline float sum(const vec3f &a) { return a.x + a.y + a.z; }

inline float mean(const vec3f &a) { return sum(a) / 3; }

// Functions applied to vector elements
inline vec3f abs(const vec3f &a) { return {abs(a.x), abs(a.y), abs(a.z)}; }

inline vec3f sqrt(const vec3f &a) { return {sqrt(a.x), sqrt(a.y), sqrt(a.z)}; }

inline vec3f exp(const vec3f &a) { return {exp(a.x), exp(a.y), exp(a.z)}; }

inline vec3f log(const vec3f &a) { return {log(a.x), log(a.y), log(a.z)}; }

inline vec3f exp2(const vec3f &a) { return {exp2(a.x), exp2(a.y), exp2(a.z)}; }

inline vec3f log2(const vec3f &a) { return {log2(a.x), log2(a.y), log2(a.z)}; }

inline vec3f pow(const vec3f &a, float b) {
    return {pow(a.x, b), pow(a.y, b), pow(a.z, b)};
}

inline vec3f pow(const vec3f &a, const vec3f &b) {
    return {pow(a.x, b.x), pow(a.y, b.y), pow(a.z, b.z)};
}

inline vec3f gain(const vec3f &a, float b) {
    return {gain(a.x, b), gain(a.y, b), gain(a.z, b)};
}

inline bool isfinite(const vec3f &a) {
    return isfinite(a.x) && isfinite(a.y) && isfinite(a.z);
}

inline void swap(vec3f &a, vec3f &b) { std::swap(a, b); }

// Vector sequence operations.
inline int size(const vec4f &a) { return 4; }

inline const float *begin(const vec4f &a) { return &a.x; }

inline const float *end(const vec4f &a) { return &a.x + 4; }

inline float *begin(vec4f &a) { return &a.x; }

inline float *end(vec4f &a) { return &a.x + 4; }

inline const float *data(const vec4f &a) { return &a.x; }

inline float *data(vec4f &a) { return &a.x; }

// Vector comparison operations.
inline bool operator==(const vec4f &a, const vec4f &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

inline bool operator!=(const vec4f &a, const vec4f &b) {
    return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
}

// Vector operations.
inline vec4f operator+(const vec4f &a) { return a; }

inline vec4f operator-(const vec4f &a) { return {-a.x, -a.y, -a.z, -a.w}; }

inline vec4f operator+(const vec4f &a, const vec4f &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

inline vec4f operator+(const vec4f &a, float b) {
    return {a.x + b, a.y + b, a.z + b, a.w + b};
}

inline vec4f operator+(float a, const vec4f &b) {
    return {a + b.x, a + b.y, a + b.z, a + b.w};
}

inline vec4f operator-(const vec4f &a, const vec4f &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

inline vec4f operator-(const vec4f &a, float b) {
    return {a.x - b, a.y - b, a.z - b, a.w - b};
}

inline vec4f operator-(float a, const vec4f &b) {
    return {a - b.x, a - b.y, a - b.z, a - b.w};
}

inline vec4f operator*(const vec4f &a, const vec4f &b) {
    return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}

inline vec4f operator*(const vec4f &a, float b) {
    return {a.x * b, a.y * b, a.z * b, a.w * b};
}

inline vec4f operator*(float a, const vec4f &b) {
    return {a * b.x, a * b.y, a * b.z, a * b.w};
}

inline vec4f operator/(const vec4f &a, const vec4f &b) {
    return {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
}

inline vec4f operator/(const vec4f &a, float b) {
    return {a.x / b, a.y / b, a.z / b, a.w / b};
}

inline vec4f operator/(float a, const vec4f &b) {
    return {a / b.x, a / b.y, a / b.z, a / b.w};
}

// Vector assignments
inline vec4f &operator+=(vec4f &a, const vec4f &b) { return a = a + b; }

inline vec4f &operator+=(vec4f &a, float b) { return a = a + b; }

inline vec4f &operator-=(vec4f &a, const vec4f &b) { return a = a - b; }

inline vec4f &operator-=(vec4f &a, float b) { return a = a - b; }

inline vec4f &operator*=(vec4f &a, const vec4f &b) { return a = a * b; }

inline vec4f &operator*=(vec4f &a, float b) { return a = a * b; }

inline vec4f &operator/=(vec4f &a, const vec4f &b) { return a = a / b; }

inline vec4f &operator/=(vec4f &a, float b) { return a = a / b; }

// Vector products and lengths.
inline float dot(const vec4f &a, const vec4f &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline float length(const vec4f &a) { return sqrt(dot(a, a)); }

inline float length_squared(const vec4f &a) { return dot(a, a); }

inline vec4f normalize(const vec4f &a) {
    auto l = length(a);
    return (l != 0) ? a / l : a;
}

inline float distance(const vec4f &a, const vec4f &b) { return length(a - b); }

inline float distance_squared(const vec4f &a, const vec4f &b) {
    return dot(a - b, a - b);
}

inline float angle(const vec4f &a, const vec4f &b) {
    return acos(clamp(dot(normalize(a), normalize(b)), (float) -1, (float) 1));
}

inline vec4f slerp(const vec4f &a, const vec4f &b, float u) {
    // https://en.wikipedia.org/wiki/Slerp
    auto an = normalize(a), bn = normalize(b);
    auto d = dot(an, bn);
    if (d < 0) {
        bn = -bn;
        d = -d;
    }
    if (d > (float) 0.9995) return normalize(an + u * (bn - an));
    auto th = acos(clamp(d, (float) -1, (float) 1));
    if (th == 0) return an;
    return an * (sin(th * (1 - u)) / sin(th)) + bn * (sin(th * u) / sin(th));
}

// Max element and clamp.
inline vec4f max(const vec4f &a, float b) {
    return {max(a.x, b), max(a.y, b), max(a.z, b), max(a.w, b)};
}

inline vec4f min(const vec4f &a, float b) {
    return {min(a.x, b), min(a.y, b), min(a.z, b), min(a.w, b)};
}

inline vec4f max(const vec4f &a, const vec4f &b) {
    return {max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w)};
}

inline vec4f min(const vec4f &a, const vec4f &b) {
    return {min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w)};
}

inline vec4f clamp(const vec4f &x, float min, float max) {
    return {clamp(x.x, min, max), clamp(x.y, min, max), clamp(x.z, min, max),
            clamp(x.w, min, max)};
}

inline vec4f lerp(const vec4f &a, const vec4f &b, float u) {
    return a * (1 - u) + b * u;
}

inline vec4f lerp(const vec4f &a, const vec4f &b, const vec4f &u) {
    return a * (1 - u) + b * u;
}

inline float max(const vec4f &a) { return max(max(max(a.x, a.y), a.z), a.w); }

inline float min(const vec4f &a) { return min(min(min(a.x, a.y), a.z), a.w); }

inline float sum(const vec4f &a) { return a.x + a.y + a.z + a.w; }

inline float mean(const vec4f &a) { return sum(a) / 4; }

// Functions applied to vector elements
inline vec4f abs(const vec4f &a) {
    return {abs(a.x), abs(a.y), abs(a.z), abs(a.w)};
}

inline vec4f sqrt(const vec4f &a) {
    return {sqrt(a.x), sqrt(a.y), sqrt(a.z), sqrt(a.w)};
}

inline vec4f exp(const vec4f &a) {
    return {exp(a.x), exp(a.y), exp(a.z), exp(a.w)};
}

inline vec4f log(const vec4f &a) {
    return {log(a.x), log(a.y), log(a.z), log(a.w)};
}

inline vec4f exp2(const vec4f &a) {
    return {exp2(a.x), exp2(a.y), exp2(a.z), exp2(a.w)};
}

inline vec4f log2(const vec4f &a) {
    return {log2(a.x), log2(a.y), log2(a.z), log2(a.w)};
}

inline vec4f pow(const vec4f &a, float b) {
    return {pow(a.x, b), pow(a.y, b), pow(a.z, b), pow(a.w, b)};
}

inline vec4f pow(const vec4f &a, const vec4f &b) {
    return {pow(a.x, b.x), pow(a.y, b.y), pow(a.z, b.z), pow(a.w, b.w)};
}

inline vec4f gain(const vec4f &a, float b) {
    return {gain(a.x, b), gain(a.y, b), gain(a.z, b), gain(a.w, b)};
}

inline bool isfinite(const vec4f &a) {
    return isfinite(a.x) && isfinite(a.y) && isfinite(a.z) && isfinite(a.w);
}

inline void swap(vec4f &a, vec4f &b) { std::swap(a, b); }

// Quaternion operatons represented as xi + yj + zk + w
// const auto identity_quat4f = vec4f{0, 0, 0, 1};
inline vec4f quat_mul(const vec4f &a, float b) {
    return {a.x * b, a.y * b, a.z * b, a.w * b};
}

inline vec4f quat_mul(const vec4f &a, const vec4f &b) {
    return {a.x * b.w + a.w * b.x + a.y * b.w - a.z * b.y,
            a.y * b.w + a.w * b.y + a.z * b.x - a.x * b.z,
            a.z * b.w + a.w * b.z + a.x * b.y - a.y * b.x,
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z};
}

inline vec4f quat_conjugate(const vec4f &a) { return {-a.x, -a.y, -a.z, a.w}; }

inline vec4f quat_inverse(const vec4f &a) {
    return quat_conjugate(a) / dot(a, a);
}

}  // namespace bcg


// -----------------------------------------------------------------------------
// INTEGER VECTORS
// -----------------------------------------------------------------------------
namespace bcg {

// Vector data types
inline int &vec2i::operator[](int i) { return (&x)[i]; }

inline const int &vec2i::operator[](int i) const { return (&x)[i]; }

// Vector data types
inline int &vec3i::operator[](int i) { return (&x)[i]; }

inline const int &vec3i::operator[](int i) const { return (&x)[i]; }

// Vector data types
inline int &vec4i::operator[](int i) { return (&x)[i]; }

inline const int &vec4i::operator[](int i) const { return (&x)[i]; }

// Vector data types
inline byte &vec3b::operator[](int i) { return (&x)[i]; }

inline const byte &vec3b::operator[](int i) const { return (&x)[i]; }

// Vector data types
inline byte &vec4b::operator[](int i) { return (&x)[i]; }

inline const byte &vec4b::operator[](int i) const { return (&x)[i]; }

// Vector data types
inline ushort &vec3s::operator[](int i) { return (&x)[i]; }

inline const ushort &vec3s::operator[](int i) const { return (&x)[i]; }

// Vector data types
inline ushort &vec4s::operator[](int i) { return (&x)[i]; }

inline const ushort &vec4s::operator[](int i) const { return (&x)[i]; }

// Element access
inline vec3i xyz(const vec4i &a) { return {a.x, a.y, a.z}; }

// Element access
inline vec3b xyz(const vec4b &a) { return {a.x, a.y, a.z}; }

// Element access
inline vec3s xyz(const vec4s &a) { return {a.x, a.y, a.z}; }

// Vector sequence operations.
inline int size(const vec2i &a) { return 2; }

inline const int *begin(const vec2i &a) { return &a.x; }

inline const int *end(const vec2i &a) { return &a.x + 2; }

inline int *begin(vec2i &a) { return &a.x; }

inline int *end(vec2i &a) { return &a.x + 2; }

inline const int *data(const vec2i &a) { return &a.x; }

inline int *data(vec2i &a) { return &a.x; }

// Vector comparison operations.
inline bool operator==(const vec2i &a, const vec2i &b) {
    return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const vec2i &a, const vec2i &b) {
    return a.x != b.x || a.y != b.y;
}

// Vector operations.
inline vec2i operator+(const vec2i &a) { return a; }

inline vec2i operator-(const vec2i &a) { return {-a.x, -a.y}; }

inline vec2i operator+(const vec2i &a, const vec2i &b) {
    return {a.x + b.x, a.y + b.y};
}

inline vec2i operator+(const vec2i &a, int b) { return {a.x + b, a.y + b}; }

inline vec2i operator+(int a, const vec2i &b) { return {a + b.x, a + b.y}; }

inline vec2i operator-(const vec2i &a, const vec2i &b) {
    return {a.x - b.x, a.y - b.y};
}

inline vec2i operator-(const vec2i &a, int b) { return {a.x - b, a.y - b}; }

inline vec2i operator-(int a, const vec2i &b) { return {a - b.x, a - b.y}; }

inline vec2i operator*(const vec2i &a, const vec2i &b) {
    return {a.x * b.x, a.y * b.y};
}

inline vec2i operator*(const vec2i &a, int b) { return {a.x * b, a.y * b}; }

inline vec2i operator*(int a, const vec2i &b) { return {a * b.x, a * b.y}; }

inline vec2i operator/(const vec2i &a, const vec2i &b) {
    return {a.x / b.x, a.y / b.y};
}

inline vec2i operator/(const vec2i &a, int b) { return {a.x / b, a.y / b}; }

inline vec2i operator/(int a, const vec2i &b) { return {a / b.x, a / b.y}; }

// Vector assignments
inline vec2i &operator+=(vec2i &a, const vec2i &b) { return a = a + b; }

inline vec2i &operator+=(vec2i &a, int b) { return a = a + b; }

inline vec2i &operator-=(vec2i &a, const vec2i &b) { return a = a - b; }

inline vec2i &operator-=(vec2i &a, int b) { return a = a - b; }

inline vec2i &operator*=(vec2i &a, const vec2i &b) { return a = a * b; }

inline vec2i &operator*=(vec2i &a, int b) { return a = a * b; }

inline vec2i &operator/=(vec2i &a, const vec2i &b) { return a = a / b; }

inline vec2i &operator/=(vec2i &a, int b) { return a = a / b; }

// Max element and clamp.
inline vec2i max(const vec2i &a, int b) { return {max(a.x, b), max(a.y, b)}; }

inline vec2i min(const vec2i &a, int b) { return {min(a.x, b), min(a.y, b)}; }

inline vec2i max(const vec2i &a, const vec2i &b) {
    return {max(a.x, b.x), max(a.y, b.y)};
}

inline vec2i min(const vec2i &a, const vec2i &b) {
    return {min(a.x, b.x), min(a.y, b.y)};
}

inline vec2i clamp(const vec2i &x, int min, int max) {
    return {clamp(x.x, min, max), clamp(x.y, min, max)};
}

inline int max(const vec2i &a) { return max(a.x, a.y); }

inline int min(const vec2i &a) { return min(a.x, a.y); }

inline int sum(const vec2i &a) { return a.x + a.y; }

// Functions applied to vector elements
inline vec2i abs(const vec2i &a) { return {abs(a.x), abs(a.y)}; }

inline void swap(vec2i &a, vec2i &b) { std::swap(a, b); }

// Vector sequence operations.
inline int size(const vec3i &a) { return 3; }

inline const int *begin(const vec3i &a) { return &a.x; }

inline const int *end(const vec3i &a) { return &a.x + 3; }

inline int *begin(vec3i &a) { return &a.x; }

inline int *end(vec3i &a) { return &a.x + 3; }

inline const int *data(const vec3i &a) { return &a.x; }

inline int *data(vec3i &a) { return &a.x; }

// Vector comparison operations.
inline bool operator==(const vec3i &a, const vec3i &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

inline bool operator!=(const vec3i &a, const vec3i &b) {
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

// Vector operations.
inline vec3i operator+(const vec3i &a) { return a; }

inline vec3i operator-(const vec3i &a) { return {-a.x, -a.y, -a.z}; }

inline vec3i operator+(const vec3i &a, const vec3i &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

inline vec3i operator+(const vec3i &a, int b) {
    return {a.x + b, a.y + b, a.z + b};
}

inline vec3i operator+(int a, const vec3i &b) {
    return {a + b.x, a + b.y, a + b.z};
}

inline vec3i operator-(const vec3i &a, const vec3i &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

inline vec3i operator-(const vec3i &a, int b) {
    return {a.x - b, a.y - b, a.z - b};
}

inline vec3i operator-(int a, const vec3i &b) {
    return {a - b.x, a - b.y, a - b.z};
}

inline vec3i operator*(const vec3i &a, const vec3i &b) {
    return {a.x * b.x, a.y * b.y, a.z * b.z};
}

inline vec3i operator*(const vec3i &a, int b) {
    return {a.x * b, a.y * b, a.z * b};
}

inline vec3i operator*(int a, const vec3i &b) {
    return {a * b.x, a * b.y, a * b.z};
}

inline vec3i operator/(const vec3i &a, const vec3i &b) {
    return {a.x / b.x, a.y / b.y, a.z / b.z};
}

inline vec3i operator/(const vec3i &a, int b) {
    return {a.x / b, a.y / b, a.z / b};
}

inline vec3i operator/(int a, const vec3i &b) {
    return {a / b.x, a / b.y, a / b.z};
}

// Vector assignments
inline vec3i &operator+=(vec3i &a, const vec3i &b) { return a = a + b; }

inline vec3i &operator+=(vec3i &a, int b) { return a = a + b; }

inline vec3i &operator-=(vec3i &a, const vec3i &b) { return a = a - b; }

inline vec3i &operator-=(vec3i &a, int b) { return a = a - b; }

inline vec3i &operator*=(vec3i &a, const vec3i &b) { return a = a * b; }

inline vec3i &operator*=(vec3i &a, int b) { return a = a * b; }

inline vec3i &operator/=(vec3i &a, const vec3i &b) { return a = a / b; }

inline vec3i &operator/=(vec3i &a, int b) { return a = a / b; }

// Max element and clamp.
inline vec3i max(const vec3i &a, int b) {
    return {max(a.x, b), max(a.y, b), max(a.z, b)};
}

inline vec3i min(const vec3i &a, int b) {
    return {min(a.x, b), min(a.y, b), min(a.z, b)};
}

inline vec3i max(const vec3i &a, const vec3i &b) {
    return {max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)};
}

inline vec3i min(const vec3i &a, const vec3i &b) {
    return {min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)};
}

inline vec3i clamp(const vec3i &x, int min, int max) {
    return {clamp(x.x, min, max), clamp(x.y, min, max), clamp(x.z, min, max)};
}

inline int max(const vec3i &a) { return max(max(a.x, a.y), a.z); }

inline int min(const vec3i &a) { return min(min(a.x, a.y), a.z); }

inline int sum(const vec3i &a) { return a.x + a.y + a.z; }

// Functions applied to vector elements
inline vec3i abs(const vec3i &a) { return {abs(a.x), abs(a.y), abs(a.z)}; }

inline void swap(vec3i &a, vec3i &b) { std::swap(a, b); }

// Vector sequence operations.
inline int size(const vec4i &a) { return 4; }

inline const int *begin(const vec4i &a) { return &a.x; }

inline const int *end(const vec4i &a) { return &a.x + 4; }

inline int *begin(vec4i &a) { return &a.x; }

inline int *end(vec4i &a) { return &a.x + 4; }

inline const int *data(const vec4i &a) { return &a.x; }

inline int *data(vec4i &a) { return &a.x; }

// Vector comparison operations.
inline bool operator==(const vec4i &a, const vec4i &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

inline bool operator!=(const vec4i &a, const vec4i &b) {
    return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
}

// Vector operations.
inline vec4i operator+(const vec4i &a) { return a; }

inline vec4i operator-(const vec4i &a) { return {-a.x, -a.y, -a.z, -a.w}; }

inline vec4i operator+(const vec4i &a, const vec4i &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

inline vec4i operator+(const vec4i &a, int b) {
    return {a.x + b, a.y + b, a.z + b, a.w + b};
}

inline vec4i operator+(int a, const vec4i &b) {
    return {a + b.x, a + b.y, a + b.z, a + b.w};
}

inline vec4i operator-(const vec4i &a, const vec4i &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

inline vec4i operator-(const vec4i &a, int b) {
    return {a.x - b, a.y - b, a.z - b, a.w - b};
}

inline vec4i operator-(int a, const vec4i &b) {
    return {a - b.x, a - b.y, a - b.z, a - b.w};
}

inline vec4i operator*(const vec4i &a, const vec4i &b) {
    return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}

inline vec4i operator*(const vec4i &a, int b) {
    return {a.x * b, a.y * b, a.z * b, a.w * b};
}

inline vec4i operator*(int a, const vec4i &b) {
    return {a * b.x, a * b.y, a * b.z, a * b.w};
}

inline vec4i operator/(const vec4i &a, const vec4i &b) {
    return {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
}

inline vec4i operator/(const vec4i &a, int b) {
    return {a.x / b, a.y / b, a.z / b, a.w / b};
}

inline vec4i operator/(int a, const vec4i &b) {
    return {a / b.x, a / b.y, a / b.z, a / b.w};
}

// Vector assignments
inline vec4i &operator+=(vec4i &a, const vec4i &b) { return a = a + b; }

inline vec4i &operator+=(vec4i &a, int b) { return a = a + b; }

inline vec4i &operator-=(vec4i &a, const vec4i &b) { return a = a - b; }

inline vec4i &operator-=(vec4i &a, int b) { return a = a - b; }

inline vec4i &operator*=(vec4i &a, const vec4i &b) { return a = a * b; }

inline vec4i &operator*=(vec4i &a, int b) { return a = a * b; }

inline vec4i &operator/=(vec4i &a, const vec4i &b) { return a = a / b; }

inline vec4i &operator/=(vec4i &a, int b) { return a = a / b; }

// Max element and clamp.
inline vec4i max(const vec4i &a, int b) {
    return {max(a.x, b), max(a.y, b), max(a.z, b), max(a.w, b)};
}

inline vec4i min(const vec4i &a, int b) {
    return {min(a.x, b), min(a.y, b), min(a.z, b), min(a.w, b)};
}

inline vec4i max(const vec4i &a, const vec4i &b) {
    return {max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w)};
}

inline vec4i min(const vec4i &a, const vec4i &b) {
    return {min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w)};
}

inline vec4i clamp(const vec4i &x, int min, int max) {
    return {clamp(x.x, min, max), clamp(x.y, min, max), clamp(x.z, min, max),
            clamp(x.w, min, max)};
}

inline int max(const vec4i &a) { return max(max(max(a.x, a.y), a.z), a.w); }

inline int min(const vec4i &a) { return min(min(min(a.x, a.y), a.z), a.w); }

inline int sum(const vec4i &a) { return a.x + a.y + a.z + a.w; }

// Functions applied to vector elements
inline vec4i abs(const vec4i &a) {
    return {abs(a.x), abs(a.y), abs(a.z), abs(a.w)};
}

inline void swap(vec4i &a, vec4i &b) { std::swap(a, b); }

}  // namespace bcg


#endif //BCG_GRAPHICS_BCG_VECTOR_H
