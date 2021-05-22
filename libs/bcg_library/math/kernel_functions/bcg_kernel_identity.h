//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_IDENTITY_H
#define BCG_GRAPHICS_BCG_KERNEL_IDENTITY_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_identity : public kernel {
    kernel_identity() : kernel() {}

    ~kernel_identity() override = default;

    bcg_scalar_t operator()(bcg_scalar_t x) const override{
        return x;
    }
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_IDENTITY_H
