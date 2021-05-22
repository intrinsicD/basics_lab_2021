//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_LOG_H
#define BCG_GRAPHICS_BCG_KERNEL_LOG_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_log : public kernel {
    explicit kernel_log(int degree) : kernel(),
                                      degree(degree) {

    }

    ~kernel_log() override = default;

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        return -std::log(std::pow(x, degree) + 1);
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        bcg_scalar_t diff = x - y;
        return operator()(diff);
    }

    bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const override {
        bcg_scalar_t diff_norm = (x - y).norm();
        return operator()(diff);
    }

    bcg_scalar_t degree;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_LOG_H
