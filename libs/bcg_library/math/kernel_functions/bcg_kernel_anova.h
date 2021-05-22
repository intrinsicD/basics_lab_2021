//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_ANOVA_H
#define BCG_GRAPHICS_BCG_KERNEL_ANOVA_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_anova : public kernel {
    kernel_anova(int k, bcg_scalar_t sigma, int degree) : kernel(), k(k),
                                                          sigma(sigma),
                                                          degree(degree) {

    }

    ~kernel_anova() override = default;

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        bcg_scalar_t result = 0;
        for (int i = 0; i < k; ++i) {
            x *= x;
            result += std::pow(std::exp(-sigma * x), degree);
        }
        return result;
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        bcg_scalar_t result = 0;
        for (int i = 0; i < k; ++i) {
            bcg_scalar_t two_xy = 2.0 * x * y;
            x *= x;
            y *= y;
            result += std::pow(std::exp(-sigma * (x - two_xy + y)), degree);
        }
        return result;
    }

    int k;
    bcg_scalar_t sigma;
    int degree;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_ANOVA_H
