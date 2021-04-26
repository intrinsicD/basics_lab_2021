//
// Created by alex on 13.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MATH_COMMON_H
#define BCG_GRAPHICS_BCG_MATH_COMMON_H

#include <cmath>
#include <cstdint>
#include <limits>
#include "utils/bcg_macros.h"

namespace bcg {

using bcg_size_t = std::int64_t;
using bcg_index_t = unsigned int;
using bcg_scalar_t = double;

BCG_MAYBE_UNUSED inline constexpr bcg_scalar_t pi = 3.14159265358979323846;

BCG_MAYBE_UNUSED inline constexpr auto index_max = std::numeric_limits<bcg_index_t>::max();
BCG_MAYBE_UNUSED inline constexpr auto index_min = std::numeric_limits<bcg_index_t>::lowest();
BCG_MAYBE_UNUSED inline constexpr auto scalar_max = std::numeric_limits<bcg_scalar_t>::max();
BCG_MAYBE_UNUSED inline constexpr auto scalar_min = std::numeric_limits<bcg_scalar_t>::lowest();
BCG_MAYBE_UNUSED inline constexpr auto scalar_eps = std::numeric_limits<bcg_scalar_t>::epsilon();


}

#endif //BCG_GRAPHICS_BCG_MATH_COMMON_H
