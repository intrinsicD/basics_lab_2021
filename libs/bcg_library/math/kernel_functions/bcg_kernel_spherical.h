//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_SPHERICAL_H
#define BCG_GRAPHICS_BCG_KERNEL_SPHERICAL_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_sphereical : public kernel {
    explicit kernel_sphereical(bcg_scalar_t sigma) : kernel(),
                                                     sigma(sigma),
                                                     sigma_cubed(sigma * sigma * sigma) {

    }

    ~kernel_sphereical() override = default;

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        bcg_scalar_t abs_x = std::abs(x);
        return 1.0 - 3.0 / 2.0 * abs_x / sigma + 0.5 * abs_x * abs_x * abs_x / sigma_cubed;
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        bcg_scalar_t diff = x - y;
        return operator()(diff);
    }

    bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const override {
        bcg_scalar_t diff_norm = (x - y).norm();
        return operator()(diff);
    }

    bcg_scalar_t sigma, sigma_cubed;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_SPHERICAL_H
