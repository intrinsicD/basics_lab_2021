//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_HYPERBOLIC_TANGENT_H
#define BCG_GRAPHICS_BCG_KERNEL_HYPERBOLIC_TANGENT_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_hyperbolic_tangent : public kernel {
    kernel_hyperbolic_tangent(bcg_scalar_t alpha, bcg_scalar_t constant) : kernel(),
                                                                           alpha(alpha),
                                                                           constant(constant) {

    }

    ~kernel_hyperbolic_tangent() override = default;

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        return std::tanh(alpha * x + c);
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        return operator()( x * y);
    }

    bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const override {
        return operator()(x.transpose() * y);
    }

    bcg_scalar_t alpha, constant;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_HYPERBOLIC_TANGENT_H
