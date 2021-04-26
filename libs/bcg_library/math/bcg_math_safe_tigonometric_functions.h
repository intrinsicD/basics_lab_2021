//
// Created by alex on 26.04.21.
//

#ifndef BCG_GRAPHICS_BCG_MATH_SAFE_TIGONOMETRIC_FUNCTIONS_H
#define BCG_GRAPHICS_BCG_MATH_SAFE_TIGONOMETRIC_FUNCTIONS_H

#include "bcg_math_common.h"

namespace bcg{

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

#endif //BCG_GRAPHICS_BCG_MATH_SAFE_TIGONOMETRIC_FUNCTIONS_H
