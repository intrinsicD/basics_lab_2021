//
// Created by alex on 22.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_GAUSSIAN_H
#define BCG_GRAPHICS_BCG_KERNEL_GAUSSIAN_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_gaussian : public kernel {
    explicit kernel_gaussian(bcg_scalar_t sigma) : kernel(),
                                                   two_sigma_squared(2.0 * sigma * sigma) {

    }

    ~kernel_exponential_quadratic() override = default;

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        return std::exp(-x * x / two_sigma_squared);
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        return operator()(x - y);
    }

    bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const override {
        return std::exp(-(x - y).squaredNorm() / two_sigma_squared));
    }

    bcg_scalar_t two_sigma_squared;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_GAUSSIAN_H
