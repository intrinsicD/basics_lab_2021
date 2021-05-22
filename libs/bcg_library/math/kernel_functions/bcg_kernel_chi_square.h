//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_CHI_SQUARE_H
#define BCG_GRAPHICS_BCG_KERNEL_CHI_SQUARE_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_chi_square : public kernel {
    kernel_chi_square() : kernel(){

    }

    ~kernel_chi_square() override = default;

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        return 1.0;
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        return 2.0 * x * y / (x + y);
    }

    bcg_scalar_t operator()(const VectorS<1> &x, const VectorS<-1> &y) const override {
        return 2.0 * ((x.array() * y.array()) / (x + y).array()).sum();
    }
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_CHI_SQUARE_H
