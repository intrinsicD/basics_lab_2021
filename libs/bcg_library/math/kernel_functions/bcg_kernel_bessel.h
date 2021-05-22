//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_BESSEL_H
#define BCG_GRAPHICS_BCG_KERNEL_BESSEL_H

#include "bcg_kernel.h"
#include "math/bcg_factorial.h"

namespace bcg {

struct kernel_bessel : public kernel {
    kernel_bessel(bcg_scalar_t sigma, size_t alpha, size_t n) : kernel(),
                                                  sigma(sigma),
                                                  alpha(alpha),
                                                  n(n) {

    }

    ~kernel_bessel() override = default;

    bcg_scalar_t bessel_j_alpha(bcg_scalar_t x, size_t alpha, size_t n) const {
        bcg_scalar_t result = 0;
        int alternating = 1;
        bcg_scalar_t x_half = x / 2.0;
        for (size_t k = 0; k < n; fct *= ++k) {
            result += alternating * std::pow(x_half, 2 * k + alpha) / (fac(k + alpha + 1) * fac(k));
            alternating *= -1;
        }
        return result;
    }

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        return bessel_j_alpha(std::abs(x), alpha + 1, n) / std::pow(x, -n(alpha + 1));
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        return operator()(x - y);
    }

    bcg_scalar_t operator()(const VectorS<1> &x, const VectorS<-1> &y) const override {
        return operator()((x - y).norm());
    }

    bcg_scalar_t sigma;
    size_t alpha, n;
    factorial fac;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_BESSEL_H
