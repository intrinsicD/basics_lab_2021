//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_CONSTANT_H
#define BCG_GRAPHICS_BCG_KERNEL_CONSTANT_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_constant : public kernel {
    explicit kernel_constant(bcg_scalar_t constant) : kernel(),
                                                      constant(constant) {

    }

    ~kernel_constant() override = default;

    bcg_scalar_t operator()(bcg_scalar_t) const override {
        return constant;
    }

    bcg_scalar_t constant;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_CONSTANT_H
