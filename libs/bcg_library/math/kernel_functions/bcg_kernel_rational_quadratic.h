//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_RATIONAL_QUADRATIC_H
#define BCG_GRAPHICS_BCG_KERNEL_RATIONAL_QUADRATIC_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_rational_quadratic : public kernel {
    explicit kernel_rational_quadratic(bcg_scalar_t constant) : kernel(),
                                                       constant(constant) {

    }

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        bcg_scalar_t x_squared = x * x;
        return 1 - x_squared / (x_squared + constant);
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        return operator()(x - y);
    }

    bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const override {
        return operator()((x - y).norm());
    }

    bcg_scalar_t constant;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_RATIONAL_QUADRATIC_H
