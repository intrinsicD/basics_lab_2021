//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_CIRCULAR_H
#define BCG_GRAPHICS_BCG_KERNEL_CIRCULAR_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_circular : public kernel {
    explicit kernel_circular(bcg_scalar_t sigma) : kernel(),
                                          sigma(sigma),
                                          sigma_squared(sigma * sigma),
                                          two_div_pi(2.0 / pi) {

    }

    ~kernel_circular() override = default;


    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        bcg_scalar_t abs_x = std::abs(x);
        if(abs_x < sigma) return 0;
        return two_div_pi * std::acos(-abs_x / sigma) -
               two_div_pi * abs_x / sigma * std::sqrt(1 - abs_x * abs_x / sigma_squared);
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        bcg_scalar_t diff = x - y;
        return operator()(diff);
    }

    bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const override {
        bcg_scalar_t diff_norm = (x - y).norm();
        return operator()(diff);
    }

    bcg_scalar_t sigma, sigma_squared;
    constexpr bcg_scalar_t two_div_pi;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_CIRCULAR_H
