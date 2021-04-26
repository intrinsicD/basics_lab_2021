//
// Created by alex on 13.10.20.
//

#ifndef BCG_GRAPHICS_BCG_TRIANGLE_METRIC_H
#define BCG_GRAPHICS_BCG_TRIANGLE_METRIC_H

#include "math/bcg_math_safe_tigonometric_functions.h"

namespace bcg {

inline bcg_scalar_t angle_from_metric(bcg_scalar_t a, bcg_scalar_t b, bcg_scalar_t c) {
    /* Numerically stable version of law of cosines
     * angle between a and b, opposite to c
    */
    auto alpha = safe_acos((a * a + b * b - c * c) / (2.0 * a * b));
    return alpha >= scalar_eps ? alpha : std::sqrt((c * c - (a - b) * (a - b)) / (a * b));
}

inline bcg_scalar_t area_from_metric(bcg_scalar_t a, bcg_scalar_t b, bcg_scalar_t c) {
    //Numerically stable herons formula
    if (a < b) std::swap(a, b);
    if (a < c) std::swap(a, c);
    if (b < c) std::swap(b, c);

    auto p = std::abs((a + (b + c)) * (c - (a - b)) * (c + (a - b)) * (a + (b - c)));
    if (p < scalar_eps) p = scalar_eps;
    return 0.25 * std::sqrt(p);
}


}

#endif //BCG_GRAPHICS_BCG_TRIANGLE_METRIC_H
