//
// Created by alex on 21.05.21.
//

#ifndef BCG_GRAPHICS_BCG_KERNEL_H
#define BCG_GRAPHICS_BCG_KERNEL_H

#include "math/bcg_linalg.h"
#include <functional>

/*
 * kernel functions:
 *
 * https://www.cs.toronto.edu/~duvenaud/cookbook/
 * http://crsouza.com/2010/03/17/kernel-functions-for-machine-learning-applications/#anova
 * */

namespace bcg {

struct kernel {
    kernel() = default;

    virtual ~kernel() = default;

    virtual bcg_scalar_t operator()(bcg_scalar_t x) const{
        return x;
    };

    virtual bcg_scalar_t operator()(bcg_scalar_t x, bcg_scalar_t y) const{
        return x * y;
    };

    virtual bcg_scalar_t operator()(const VectorS<-1> &x, const VectorS<-1> &y) const{
        return x.transpose() * y;
    }
};

}

#endif //BCG_GRAPHICS_BCG_KERNEL_H
