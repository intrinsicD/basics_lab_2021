//
// Created by alex on 12.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MONOMIAL_BASIS_H
#define BCG_GRAPHICS_BCG_MONOMIAL_BASIS_H

#include <vector>
#include "bcg_math_types.h"

namespace bcg{

struct monomial_basis{
    static std::vector<bcg_scalar_t> evaluate(bcg_scalar_t t, int degree);

    static std::vector<bcg_scalar_t> derivative(bcg_scalar_t t, int degree, int order);

    static std::vector<bcg_scalar_t> integrate(bcg_scalar_t t, int degree, int order);
};

}

#endif //BCG_GRAPHICS_BCG_MONOMIAL_BASIS_H
