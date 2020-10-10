//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MATRIX_H
#define BCG_GRAPHICS_BCG_MATRIX_H

// -----------------------------------------------------------------------------
// MATRICES
// -----------------------------------------------------------------------------
namespace bcg {

// Small Fixed-size matrices stored in column major format.
struct mat2f {
    vec2f x = {1, 0};
    vec2f y = {0, 1};

    vec2f &operator[](int i);

    const vec2f &operator[](int i) const;
};

// Small Fixed-size matrices stored in column major format.
struct mat3f {
    vec3f x = {1, 0, 0};
    vec3f y = {0, 1, 0};
    vec3f z = {0, 0, 1};

    vec3f &operator[](int i);

    const vec3f &operator[](int i) const;
};

// Small Fixed-size matrices stored in column major format.
struct mat4f {
    vec4f x = {1, 0, 0, 0};
    vec4f y = {0, 1, 0, 0};
    vec4f z = {0, 0, 1, 0};
    vec4f w = {0, 0, 0, 1};

    vec4f &operator[](int i);

    const vec4f &operator[](int i) const;
};

// Identity matrices constants.
inline const auto identity2x2f = mat2f{{1, 0},
                                       {0, 1}};
inline const auto identity3x3f = mat3f{{1, 0, 0},
                                       {0, 1, 0},
                                       {0, 0, 1}};
inline const auto identity4x4f = mat4f{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}};

// Matrix comparisons.
inline bool operator==(const mat2f &a, const mat2f &b);

inline bool operator!=(const mat2f &a, const mat2f &b);

// Matrix operations.
inline mat2f operator+(const mat2f &a, const mat2f &b);

inline mat2f operator*(const mat2f &a, float b);

inline vec2f operator*(const mat2f &a, const vec2f &b);

inline vec2f operator*(const vec2f &a, const mat2f &b);

inline mat2f operator*(const mat2f &a, const mat2f &b);

// Matrix assignments.
inline mat2f &operator+=(mat2f &a, const mat2f &b);

inline mat2f &operator*=(mat2f &a, const mat2f &b);

inline mat2f &operator*=(mat2f &a, float b);

// Matrix diagonals and transposes.
inline vec2f diagonal(const mat2f &a);

inline mat2f transpose(const mat2f &a);

// Matrix adjoints, determinants and inverses.
inline float determinant(const mat2f &a);

inline mat2f adjoint(const mat2f &a);

inline mat2f inverse(const mat2f &a);

// Matrix comparisons.
inline bool operator==(const mat3f &a, const mat3f &b);

inline bool operator!=(const mat3f &a, const mat3f &b);

// Matrix operations.
inline mat3f operator+(const mat3f &a, const mat3f &b);

inline mat3f operator*(const mat3f &a, float b);

inline vec3f operator*(const mat3f &a, const vec3f &b);

inline vec3f operator*(const vec3f &a, const mat3f &b);

inline mat3f operator*(const mat3f &a, const mat3f &b);

// Matrix assignments.
inline mat3f &operator+=(mat3f &a, const mat3f &b);

inline mat3f &operator*=(mat3f &a, const mat3f &b);

inline mat3f &operator*=(mat3f &a, float b);

// Matrix diagonals and transposes.
inline vec3f diagonal(const mat3f &a);

inline mat3f transpose(const mat3f &a);

// Matrix adjoints, determinants and inverses.
inline float determinant(const mat3f &a);

inline mat3f adjoint(const mat3f &a);

inline mat3f inverse(const mat3f &a);

// Constructs a basis from a direction
inline mat3f basis_fromz(const vec3f &v);

// Matrix comparisons.
inline bool operator==(const mat4f &a, const mat4f &b);

inline bool operator!=(const mat4f &a, const mat4f &b);

// Matrix operations.
inline mat4f operator+(const mat4f &a, const mat4f &b);

inline mat4f operator*(const mat4f &a, float b);

inline vec4f operator*(const mat4f &a, const vec4f &b);

inline vec4f operator*(const vec4f &a, const mat4f &b);

inline mat4f operator*(const mat4f &a, const mat4f &b);

// Matrix assignments.
inline mat4f &operator+=(mat4f &a, const mat4f &b);

inline mat4f &operator*=(mat4f &a, const mat4f &b);

inline mat4f &operator*=(mat4f &a, float b);

// Matrix diagonals and transposes.
inline vec4f diagonal(const mat4f &a);

inline mat4f transpose(const mat4f &a);
}

// -----------------------------------------------------------------------------
//
//
// IMPLEMENTATION
//
//
// -----------------------------------------------------------------------------

namespace bcg {

// Small Fixed-size matrices stored in column major format.
inline vec2f &mat2f::operator[](int i) { return (&x)[i]; }

inline const vec2f &mat2f::operator[](int i) const { return (&x)[i]; }

// Small Fixed-size matrices stored in column major format.
inline vec3f &mat3f::operator[](int i) { return (&x)[i]; }

inline const vec3f &mat3f::operator[](int i) const { return (&x)[i]; }

// Small Fixed-size matrices stored in column major format.
inline vec4f &mat4f::operator[](int i) { return (&x)[i]; }

inline const vec4f &mat4f::operator[](int i) const { return (&x)[i]; }

// Matrix comparisons.
inline bool operator==(const mat2f &a, const mat2f &b) {
    return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const mat2f &a, const mat2f &b) { return !(a == b); }

// Matrix operations.
inline mat2f operator+(const mat2f &a, const mat2f &b) {
    return {a.x + b.x, a.y + b.y};
}

inline mat2f operator*(const mat2f &a, float b) { return {a.x * b, a.y * b}; }

inline vec2f operator*(const mat2f &a, const vec2f &b) {
    return a.x * b.x + a.y * b.y;
}

inline vec2f operator*(const vec2f &a, const mat2f &b) {
    return {dot(a, b.x), dot(a, b.y)};
}

inline mat2f operator*(const mat2f &a, const mat2f &b) {
    return {a * b.x, a * b.y};
}

// Matrix assignments.
inline mat2f &operator+=(mat2f &a, const mat2f &b) { return a = a + b; }

inline mat2f &operator*=(mat2f &a, const mat2f &b) { return a = a * b; }

inline mat2f &operator*=(mat2f &a, float b) { return a = a * b; }

// Matrix diagonals and transposes.
inline vec2f diagonal(const mat2f &a) { return {a.x.x, a.y.y}; }

inline mat2f transpose(const mat2f &a) {
    return {{a.x.x, a.y.x},
            {a.x.y, a.y.y}};
}

// Matrix adjoints, determinants and inverses.
inline float determinant(const mat2f &a) { return cross(a.x, a.y); }

inline mat2f adjoint(const mat2f &a) {
    return {{a.y.y,  -a.x.y},
            {-a.y.x, a.x.x}};
}

inline mat2f inverse(const mat2f &a) {
    return adjoint(a) * (1 / determinant(a));
}

// Matrix comparisons.
inline bool operator==(const mat3f &a, const mat3f &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

inline bool operator!=(const mat3f &a, const mat3f &b) { return !(a == b); }

// Matrix operations.
inline mat3f operator+(const mat3f &a, const mat3f &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

inline mat3f operator*(const mat3f &a, float b) {
    return {a.x * b, a.y * b, a.z * b};
}

inline vec3f operator*(const mat3f &a, const vec3f &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline vec3f operator*(const vec3f &a, const mat3f &b) {
    return {dot(a, b.x), dot(a, b.y), dot(a, b.z)};
}

inline mat3f operator*(const mat3f &a, const mat3f &b) {
    return {a * b.x, a * b.y, a * b.z};
}

// Matrix assignments.
inline mat3f &operator+=(mat3f &a, const mat3f &b) { return a = a + b; }

inline mat3f &operator*=(mat3f &a, const mat3f &b) { return a = a * b; }

inline mat3f &operator*=(mat3f &a, float b) { return a = a * b; }

// Matrix diagonals and transposes.
inline vec3f diagonal(const mat3f &a) { return {a.x.x, a.y.y, a.z.z}; }

inline mat3f transpose(const mat3f &a) {
    return {
            {a.x.x, a.y.x, a.z.x},
            {a.x.y, a.y.y, a.z.y},
            {a.x.z, a.y.z, a.z.z},
    };
}

// Matrix adjoints, determinants and inverses.
inline float determinant(const mat3f &a) { return dot(a.x, cross(a.y, a.z)); }

inline mat3f adjoint(const mat3f &a) {
    return transpose(mat3f{cross(a.y, a.z), cross(a.z, a.x), cross(a.x, a.y)});
}

inline mat3f inverse(const mat3f &a) {
    return adjoint(a) * (1 / determinant(a));
}

// Constructs a basis from a direction
inline mat3f basis_fromz(const vec3f &v) {
    // https://graphics.pixar.com/library/OrthonormalB/paper.pdf
    auto z = normalize(v);
    auto sign = std::copysignf(1.0f, z.z);
    auto a = -1.0f / (sign + z.z);
    auto b = z.x * z.y * a;
    auto x = vec3f{1.0f + sign * z.x * z.x * a, sign * b, -sign * z.x};
    auto y = vec3f{b, sign + z.y * z.y * a, -z.y};
    return {x, y, z};
}

// Matrix comparisons.
inline bool operator==(const mat4f &a, const mat4f &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

inline bool operator!=(const mat4f &a, const mat4f &b) { return !(a == b); }

// Matrix operations.
inline mat4f operator+(const mat4f &a, const mat4f &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

inline mat4f operator*(const mat4f &a, float b) {
    return {a.x * b, a.y * b, a.z * b, a.w * b};
}

inline vec4f operator*(const mat4f &a, const vec4f &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline vec4f operator*(const vec4f &a, const mat4f &b) {
    return {dot(a, b.x), dot(a, b.y), dot(a, b.z), dot(a, b.w)};
}

inline mat4f operator*(const mat4f &a, const mat4f &b) {
    return {a * b.x, a * b.y, a * b.z, a * b.w};
}

// Matrix assignments.
inline mat4f &operator+=(mat4f &a, const mat4f &b) { return a = a + b; }

inline mat4f &operator*=(mat4f &a, const mat4f &b) { return a = a * b; }

inline mat4f &operator*=(mat4f &a, float b) { return a = a * b; }

// Matrix diagonals and transposes.
inline vec4f diagonal(const mat4f &a) { return {a.x.x, a.y.y, a.z.z, a.w.w}; }

inline mat4f transpose(const mat4f &a) {
    return {
            {a.x.x, a.y.x, a.z.x, a.w.x},
            {a.x.y, a.y.y, a.z.y, a.w.y},
            {a.x.z, a.y.z, a.z.z, a.w.z},
            {a.x.w, a.y.w, a.z.w, a.w.w},
    };
}

}  // namespace bcg


#endif //BCG_GRAPHICS_BCG_MATRIX_H
