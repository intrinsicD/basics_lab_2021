//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_POLYNOMIAL_H
#define BCG_GRAPHICS_BCG_KERNEL_POLYNOMIAL_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_polynomial : public kernel {
    explicit kernel_polynomial(bcg_scalar_t alpha, bcg_scalar_t constant, int degree) : kernel(),
                                                                                        alpha(alpha),
                                                                                        constant(constant),
                                                                                        degree(degree) {

    }

    ~kernel_polynomial() override = default;

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        return std::pow(alpha * x + constant, degree);
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        return std::pow(alpha * x * y + constant, degree);
    }

    bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const override {
        return std::pow(alpha * x.transpose() * y + constant, degree);
    }

    bcg_scalar_t alpha, constant;
    int degree;
};

}
#endif //BCG_GRAPHICS_BCG_KERNEL_POLYNOMIAL_H
