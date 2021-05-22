//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_LINEAR_H
#define BCG_GRAPHICS_BCG_KERNEL_LINEAR_H

#include "bcg_kernel.h"

namespace bcg{

struct kernel_linear : public kernel{
    explicit kernel_linear(bcg_scalar_t constant) : kernel(), constant(constant) {

    }

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        return x + constant;
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        return x * y + constant;
    }

    bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const override {
        return x.transpose() * y + constant;
    }

    ~kernel_linear() override = default;

    bcg_scalar_t constant;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_LINEAR_H
