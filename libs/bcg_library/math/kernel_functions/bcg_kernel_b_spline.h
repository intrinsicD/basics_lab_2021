//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_B_SPLINE_H
#define BCG_GRAPHICS_BCG_KERNEL_B_SPLINE_H

#include "bcg_kernel.h"
#include "math/bcg_factorial.h"
#include "math/bcg_binomial_coefficient.h"

namespace bcg {

struct kernel_b_spline : public kernel {
    explicit kernel_b_spline(size_t n) : kernel(),
                                         n(n),
                                         n_plus_one_half((n + 1) / 2.0) {

    }

    ~kernel_b_spline() override = default;

    bcg_scalar_t B_n(size_t n, bcg_scalar_t x) const {
        bcg_scalar_t result = 0;
        int alternating = 1;
        for (size_t k = 0; k < n; ++k) {
            bcg_scalar_t term = x + n_plus_one_half - k;
            if (term <= 0) continue;
            result += bc(n + 1, k) * alternating * std::pow(term, n);
            alternating *= -1;
        }
        factorial fac;
        return result / fac(n);
    }

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        return B_n(2 * n + 1, x);
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        return operator()(x - y);
    }

    bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const override {
        bcg_scalar_t result = 1.0;
        for (int i = 0; i < x.size(); ++i) {
            result *= operator()(x(i), y(i));
        }
        return result;
    }

    size_t n;
    bcg_scalar_t n_plus_one_half;
    binomial_coefficient bc;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_B_SPLINE_H
