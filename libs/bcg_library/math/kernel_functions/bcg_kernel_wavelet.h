//
// Created by alex on 22.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_WAVELET_H
#define BCG_GRAPHICS_BCG_KERNEL_WAVELET_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_wavelet : public kernel {
    explicit kernel_wavelet(bcg_scalar_t a, bcg_scalar_t c, bool translation_invariant = false) : kernel(),
                                                                                                  translation_invariant(
                                                                                                          translation_invariant),
                                                                                                  a(a),
                                                                                                  c(c) {

    }

    ~bcg_kernel_wave() override = default;

    bcg_scalar_t h(x) const {
        return std::cos(1.75 * x) * std::exp(-x * x / 2.0);
    }

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        return h((x - c) / a);
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        if (translation_invariant) {
            bcg_scalar_t diff = x - y;
            return operator()(diff + c);
        } else {
            return operator()(x) * operator()(y);
        };
    }

    bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const override {
        if (translation_invariant) {
            return ((1.75 * (x - y)).array().cos() * (-(x - y) / 2).array().exp()).prod();
        } else {
            return ((1.75 * (x - c) / a).array().cos() * (-(x - c).array().squared() / (2 * a * a)).array().exp() *
                    (1.75 * (y - c) / a).array().cos() * (-(y - c).array().squared() / (2 * a * a)).array().exp()).prod();
        };
    }

    bool translation_invariant;
    bcg_scalar_t a, c;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_WAVELET_H
