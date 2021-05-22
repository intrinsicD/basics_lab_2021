//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_MULTIQUADRIC_H
#define BCG_GRAPHICS_BCG_KERNEL_MULTIQUADRIC_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_multiquadric : public kernel {
    kernel_multiquadric(bcg_scalar_t constant) : kernel(), constant_squared(constant * constant) {

    }

    ~kernel_multiquadric() override = default;


    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        return std::sqrt(x * x + constant_squared);
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        return operator()(x - y);
    }

    bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const override {
        return std::sqrt((x - y).squaredNorm() + constant_squared);
    }

    bcg_scalar_t constant_squared;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_MULTIQUADRIC_H
