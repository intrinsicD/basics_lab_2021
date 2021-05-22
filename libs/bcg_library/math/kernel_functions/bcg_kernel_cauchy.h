//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_CAUCHY_H
#define BCG_GRAPHICS_BCG_KERNEL_CAUCHY_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_cauchy : public kernel {
    kernel_cauchy(bcg_scalar_t sigma) : kernel(),
                                        sigma_squared(sigma) {

    }

    ~kernel_cauchy() override = default;

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        return 1.0 / (1 + x * x / sigma_squared);
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        return operator()(x - y);
    }

    bcg_scalar_t operator()(const VectorS<1> &x, const VectorS<-1> &y) const override {
        return 1.0 / (1 + (x - y).squaredNorm() / sigma_squared);
    }

    bcg_scalar_t sigma_squared;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_CAUCHY_H
