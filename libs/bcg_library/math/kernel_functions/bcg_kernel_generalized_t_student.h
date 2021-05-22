//
// Created by alex on 22.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_GENERALIZED_T_STUDENT_H
#define BCG_GRAPHICS_BCG_KERNEL_GENERALIZED_T_STUDENT_H

#include "bcg_kernel.h"

namespace bcg {

struct kernel_generalized_t_student : public kernel {
    explicit kernel_generalized_t_student(int power) : kernel(),
                                                       power(power) {

    }

    ~kernel_generalized_t_student() override = default;

    bcg_scalar_t operator()(bcg_scalar_t x) const override {
        return 1.0 / (1.0 + std::pow(std::abs(x), power));
    }

    bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const override {
        return operator()(x - y);
    }

    bcg_scalar_t operator()(const VectorS<1> &x, const VectorS<-1> &y) const override {
        return operator()((x - y).norm());
    }

    int power;
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_GENERALIZED_T_STUDENT_H
