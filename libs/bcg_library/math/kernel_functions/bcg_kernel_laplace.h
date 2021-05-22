//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_LAPLACE_H
#define BCG_GRAPHICS_BCG_KERNEL_LAPLACE_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_laplace : public kernel {
    explicit kernel_laplace(bcg_scalar_t sigma) : kernel(),
                                                  sigma(sigma) {

    }

    ~kernel_laplace() override = default;

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        return std::exp(-std::abs(x) / sigma);
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        return operator()(x - y);
    }

    bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const override {
        return operator()((x - y).norm());
    }

    bcg_scalar_t sigma;
};
}

#endif //BCG_GRAPHICS_BCG_KERNEL_LAPLACE_H
