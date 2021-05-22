//
// Created by alex on 22.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_WELSCH_H
#define BCG_GRAPHICS_BCG_KERNEL_WELSCH_H


#include "bcg_kernel.h"

namespace bcg {

struct kernel_welsch : public kernel {
    explicit kernel_welsch(bcg_scalar_t sigma) : kernel(),
                                                   two_sigma_squared(2.0 * sigma * sigma) {

    }

    ~kernel_welsch() override = default;

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        return 1.0 - std::exp(-x * x / two_sigma_squared);
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        return operator()(x - y);
    }

    bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const override {
        return 1.0 - std::exp(-(x - y).squaredNorm() / two_sigma_squared));
    }

    bcg_scalar_t two_sigma_squared;
};


}

#endif //BCG_GRAPHICS_BCG_KERNEL_WELSCH_H
