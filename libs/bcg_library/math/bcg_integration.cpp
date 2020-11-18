//
// Created by alex on 18.11.20.
//

#include <array>
#include "bcg_integration.h"

namespace bcg {


bcg_scalar_t integration::trapezoidal_rule(size_t num_samples, bcg_scalar_t a, bcg_scalar_t b,
                                           const std::function<bcg_scalar_t(bcg_scalar_t)> &integrand) {
    bcg_scalar_t h = (b - a) / (bcg_scalar_t) (num_samples - 1);
    bcg_scalar_t result = 0.5 * (integrand(a) + integrand(b));
    for (size_t i = 1; i <= num_samples - 2; ++i) {
        result += integrand(a + i * h);
    }
    result *= h;
    return result;
}

bcg_scalar_t integration::romberg_rule(size_t order, bcg_scalar_t a, bcg_scalar_t b,
                                       const std::function<bcg_scalar_t(bcg_scalar_t)> &integrand) {
    bcg_scalar_t half = 0.5;
    std::vector<std::array<bcg_scalar_t, 2>> rom(order);
    bcg_scalar_t h = b - a;
    rom[0][0] = half * h * (integrand(a) + integrand(b));
    for (size_t i0 = 2, p0 = 1; i0 <= order; ++i0, p0 *= 2) {
        // Approximations via the trapezoid rule.
        bcg_scalar_t sum = 0;
        for (size_t i1 = 1; i1 <= p0; ++i1) {
            sum += integrand(a + h * (i1 - half));
        }

        // Richardson extrapolation.
        rom[0][1] = half * (rom[0][0] + h * sum);
        for (size_t i2 = 1, p2 = 4.0; i2 < i0; ++i2, p2 *= 4) {
            rom[i2][1] = ((bcg_scalar_t)p2 * rom[i2 - 1][1] - rom[i2 - 1][0]) / bcg_scalar_t(p2 - 1);
        }

        for (size_t i1 = 0; i1 < i0; ++i1) {
            rom[i1][0] = rom[i1][1];
        }
        h *= half;
    }

    bcg_scalar_t result = rom[order - 1][0];
    return result;
}


}