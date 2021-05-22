//
// Created by alex on 22.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_GENERALIZED_HISTOGRAM_INTERSECTION_H
#define BCG_GRAPHICS_BCG_KERNEL_GENERALIZED_HISTOGRAM_INTERSECTION_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_histogram : public kernel {
    kernel_histogram(int alpha, int beta) : kernel(),
                                            alpha(alpha),
                                            beta(beta) {

    }

    ~kernel_histogram() override = default;

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        return x;
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        return std::min(std::pow(std::abs(x), alpha), std::pow(std::abs(y), beta));
    }

    bcg_scalar_t operator()(const VectorS<1> &x, const VectorS<-1> &y) const override {
        return x.cwiseAbs().array().pow(alpha).cwiseMin(y.cwiseAbs().array().pow(beta)).sum();
    }

    int alpha, beta;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_GENERALIZED_HISTOGRAM_INTERSECTION_H
