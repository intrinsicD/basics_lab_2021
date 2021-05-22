//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_POWER_H
#define BCG_GRAPHICS_BCG_KERNEL_POWER_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_power : public kernel {
    explicit kernel_power(int degree) : kernel(),
                                        degree(degree) {

    }

    ~bcg_kernel_power() override = default;

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        return -std::pow(std::abs(x), degree)
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        bcg_scalar_t diff = x - y;
        return operator()(diff);
    }

    bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const override {
        bcg_scalar_t diff_norm = (x - y).norm();
        return operator()(diff);
    }

    int degree;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_POWER_H
