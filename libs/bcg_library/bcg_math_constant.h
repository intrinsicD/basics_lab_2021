//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MATH_CONSTANTS_H
#define BCG_GRAPHICS_BCG_MATH_CONSTANTS_H

#include <limits>
#include <cmath>

// -----------------------------------------------------------------------------
// MATH CONSTANTS AND FUNCTIONS
// -----------------------------------------------------------------------------
namespace bcg {

using byte = unsigned char;
using uint = unsigned int;
using ushort = uint16_t;

inline const double pi = 3.14159265358979323846;
inline const float pif = (float) pi;

inline const auto int_max = std::numeric_limits<int>::max();
inline const auto int_min = std::numeric_limits<int>::lowest();
inline const auto flt_max = std::numeric_limits<float>::max();
inline const auto flt_min = std::numeric_limits<float>::lowest();
inline const auto flt_eps = std::numeric_limits<float>::epsilon();

inline float abs(float a);

inline float min(float a, float b);

inline float max(float a, float b);

inline float clamp(float a, float min, float max);

inline float sign(float a);

inline float sqrt(float a);

inline float sin(float a);

inline float cos(float a);

inline float tan(float a);

inline float asin(float a);

inline float acos(float a);

inline float atan(float a);

inline float log(float a);

inline float exp(float a);

inline float log2(float a);

inline float exp2(float a);

inline float pow(float a, float b);

inline bool isfinite(float a);

inline float atan2(float a, float b);

inline float fmod(float a, float b);

inline float radians(float a);

inline float degrees(float a);

inline float lerp(float a, float b, float u);

inline void swap(float &a, float &b);

inline float smoothstep(float a, float b, float u);

inline float bias(float a, float bias);

inline float gain(float a, float gain);

inline int abs(int a);

inline int min(int a, int b);

inline int max(int a, int b);

inline int clamp(int a, int min, int max);

inline int sign(int a);

inline int pow2(int a);

inline void swap(int &a, int &b);

}

// -----------------------------------------------------------------------------
//
//
// IMPLEMENTATION
//
//
// -----------------------------------------------------------------------------

namespace bcg {

inline float abs(float a) { return a < 0 ? -a : a; }

inline float min(float a, float b) { return (a < b) ? a : b; }

inline float max(float a, float b) { return (a > b) ? a : b; }

inline float clamp(float a, float min_, float max_) { return min(max(a, min_), max_); }

inline float sign(float a) { return a < 0 ? -1 : 1; }

inline float sqrt(float a) { return std::sqrt(a); }

inline float sin(float a) { return std::sin(a); }

inline float cos(float a) { return std::cos(a); }

inline float tan(float a) { return std::tan(a); }

inline float asin(float a) { return std::asin(a); }

inline float acos(float a) { return std::acos(a); }

inline float atan(float a) { return std::atan(a); }

inline float log(float a) { return std::log(a); }

inline float exp(float a) { return std::exp(a); }

inline float log2(float a) { return std::log2(a); }

inline float exp2(float a) { return std::exp2(a); }

inline float pow(float a, float b) { return std::pow(a, b); }

inline bool isfinite(float a) { return std::isfinite(a); }

inline float atan2(float a, float b) { return std::atan2(a, b); }

inline float fmod(float a, float b) { return std::fmod(a, b); }

inline void swap(float &a, float &b) { std::swap(a, b); }

inline float radians(float a) { return a * pif / 180; }

inline float degrees(float a) { return a * 180 / pif; }

inline float lerp(float a, float b, float u) { return a * (1 - u) + b * u; }

inline float step(float a, float u) { return u < a ? 0 : 1; }

inline float smoothstep(float a, float b, float u) {
    auto t = clamp((u - a) / (b - a), 0.0f, 1.0f);
    return t * t * (3 - 2 * t);
}

inline float bias(float a, float bias) {
    return a / ((1 / bias - 2) * (1 - a) + 1);
}

inline float gain(float a, float gain) {
    return (a < 0.5f) ? bias(a * 2, gain) / 2
                      : bias(a * 2 - 1, 1 - gain) / 2 + 0.5f;
}

inline int abs(int a) { return a < 0 ? -a : a; }

inline int min(int a, int b) { return (a < b) ? a : b; }

inline int max(int a, int b) { return (a > b) ? a : b; }

inline int clamp(int a, int min_, int max_) { return min(max(a, min_), max_); }

inline int sign(int a) { return a < 0 ? -1 : 1; }

inline int pow2(int a) { return 1 << a; }

inline void swap(int &a, int &b) { std::swap(a, b); }

}

#endif //BCG_GRAPHICS_BCG_MATH_CONSTANTS_H
