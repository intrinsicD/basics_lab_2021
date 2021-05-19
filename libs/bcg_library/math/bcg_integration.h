//
// Created by alex on 18.11.20.
//

#ifndef BCG_GRAPHICS_BCG_INTEGRATION_H
#define BCG_GRAPHICS_BCG_INTEGRATION_H

#include <functional>
#include "bcg_math_types.h"

namespace bcg{

struct integration{
    static bcg_scalar_t trapezoidal_rule(size_t num_samples, bcg_scalar_t a, bcg_scalar_t b, const std::function<bcg_scalar_t(bcg_scalar_t)> &integrand);
    static bcg_scalar_t romberg_rule(size_t order, bcg_scalar_t a, bcg_scalar_t b, const std::function<bcg_scalar_t(bcg_scalar_t)> &integrand);
};

}

#endif //BCG_GRAPHICS_BCG_INTEGRATION_H
