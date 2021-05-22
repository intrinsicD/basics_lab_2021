//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_SPLINE_H
#define BCG_GRAPHICS_BCG_KERNEL_SPLINE_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_spline : public kernel {
    explicit kernel_spline() : kernel() {

    }

    ~kernel_spline() override = default;

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        bcg_scalar_t x_squared = x * x;
        bcg_scalar_t x_cubed = x_squared * x;
        return 1 + x + x_squared - x / 2.0 * x_squared + 1.0 / 3.0 * x_cubed;
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        bcg_scalar_t xy = x * y;
        bcg_scalar_t min = std::min(x, y);
        bcg_scalar_t min_suared = min * min;
        bcg_scalar_t min_cubed = min_suared * min;
        return 1 + xy * xy * min - (x + y) / 2.0 * min_squared + 1.0 / 3.0 * min_cubed;
    }

    bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const override {
        bcg_scalar_t result = 1.0;
        for(int i = 0; i < x.size(); ++i){
            result *= operator()(x(i), y(i));
        }
        return result;
    }
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_SPLINE_H
