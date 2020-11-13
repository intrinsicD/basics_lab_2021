//
// Created by alex on 12.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MONOMIAL_CURVE_H
#define BCG_GRAPHICS_BCG_MONOMIAL_CURVE_H

#include "bcg_parametric_curve.h"

namespace bcg {

struct monomial_curve : public parametric_curve<monomial_curve> {
    monomial_curve();

    VectorS<3> evaluate(bcg_scalar_t t) override;

    monomial_curve derivative(int order) override;

    VectorS<3> derivative(bcg_scalar_t t, int order) override;
};

}

#endif //BCG_GRAPHICS_BCG_MONOMIAL_CURVE_H
