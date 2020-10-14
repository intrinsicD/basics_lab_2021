//
// Created by alex on 13.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MATH_COMMON_H
#define BCG_GRAPHICS_BCG_MATH_COMMON_H

#include <cmath>

namespace bcg {

[[maybe_unused]] inline constexpr double pi = 3.14159265358979323846;
[[maybe_unused]] inline constexpr float pif = (float)pi;

[[maybe_unused]] inline constexpr auto int_max = std::numeric_limits<int>::max();
[[maybe_unused]] inline constexpr auto int_min = std::numeric_limits<int>::lowest();
[[maybe_unused]] inline constexpr auto flt_max = std::numeric_limits<float>::max();
[[maybe_unused]] inline constexpr auto flt_min = std::numeric_limits<float>::lowest();
[[maybe_unused]] inline constexpr auto flt_eps = std::numeric_limits<float>::epsilon();

template<typename Real>
inline Real safe_acos(Real value) {
    if (value > 1.0) {
        return 0;
    } else if (value < -1.0) {
        return pi;
    } else {
        return std::acos(value);
    }
}

template<typename Real>
inline Real safe_asin(Real value) {
    if (value > 1.0) {
        return pi / 2;
    } else if (value < -1.0) {
        return pi / 2;
    } else {
        return std::asin(value);
    }
}

}

#endif //BCG_GRAPHICS_BCG_MATH_COMMON_H
