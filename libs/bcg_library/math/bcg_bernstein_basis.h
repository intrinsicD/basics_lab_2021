//
// Created by alex on 12.11.20.
//

#ifndef BCG_GRAPHICS_BCG_BERNSTEIN_BASIS_H
#define BCG_GRAPHICS_BCG_BERNSTEIN_BASIS_H

#include "bcg_math_common.h"

namespace bcg{

struct bernstein_basis{
    bernstein_basis();

    bernstein_basis(int degree);

    bcg_scalar_t evaluate(int i, bcg_scalar_t t);

    bcg_scalar_t derivative(int i, bcg_scalar_t t);

    int degree;
};
}

#endif //BCG_GRAPHICS_BCG_BERNSTEIN_BASIS_H
