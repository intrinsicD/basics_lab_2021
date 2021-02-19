//
// Created by alex on 13.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MATH_COMMON_H
#define BCG_GRAPHICS_BCG_MATH_COMMON_H

#include <cmath>
#include <limits>

namespace bcg {
using byte = unsigned int;
using bcg_index_t = unsigned int;
using bcg_scalar_t = double;

[[maybe_unused]] inline constexpr bcg_scalar_t pi = 3.14159265358979323846;

[[maybe_unused]] inline constexpr auto index_max = std::numeric_limits<bcg_index_t>::max();
[[maybe_unused]] inline constexpr auto index_min = std::numeric_limits<bcg_index_t>::lowest();
[[maybe_unused]] inline constexpr auto scalar_max = std::numeric_limits<bcg_scalar_t>::max();
[[maybe_unused]] inline constexpr auto scalar_min = std::numeric_limits<bcg_scalar_t>::lowest();
[[maybe_unused]] inline constexpr auto scalar_eps = std::numeric_limits<bcg_scalar_t>::epsilon();

inline bcg_scalar_t safe_acos(bcg_scalar_t value) {
    if (value > bcg_scalar_t(1)) {
        return 0;
    } else if (value < bcg_scalar_t(-1)) {
        return pi;
    } else {
        return std::acos(value);
    }
}

inline bcg_scalar_t safe_asin(bcg_scalar_t value) {
    if (value > bcg_scalar_t(1)) {
        return pi / bcg_scalar_t(2.0);
    } else if (value < bcg_scalar_t(-1)) {
        return pi / bcg_scalar_t(2);
    } else {
        return std::asin(value);
    }
}

}

#endif //BCG_GRAPHICS_BCG_MATH_COMMON_H
