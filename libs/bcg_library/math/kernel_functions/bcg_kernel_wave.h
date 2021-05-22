//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_WAVE_H
#define BCG_GRAPHICS_BCG_KERNEL_WAVE_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_wave : public kernel {
    explicit kernel_wave(bcg_scalar_t sigma) : kernel(),
                                                   sigma(sigma) {

    }

    ~bcg_kernel_wave() override = default;

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        bcg_scalar_t term = sigma / std::abs(x);
        return term * std::sin(1.0 / term);
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        bcg_scalar_t diff = x - y;
        return operator()(diff);
    }

    bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const override {
        bcg_scalar_t diff_norm = (x - y).norm();
        return operator()(diff);
    }

    bcg_scalar_t sigma;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_WAVE_H
