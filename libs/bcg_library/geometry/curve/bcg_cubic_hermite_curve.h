//
// Created by alex on 12.11.20.
//

#ifndef BCG_GRAPHICS_BCG_CUBIC_HERMITE_CURVE_H
#define BCG_GRAPHICS_BCG_CUBIC_HERMITE_CURVE_H

#include "bcg_parametric_curve.h"

namespace bcg{

struct cubic_hermite_curve : public parametric_curve<cubic_hermite_curve>{
    cubic_hermite_curve();

    VectorS<3> evaluate(bcg_scalar_t t) override;

    cubic_hermite_curve derivative(int order) override;

    VectorS<3> derivative(bcg_scalar_t t, int order) override;
};

}
#endif //BCG_GRAPHICS_BCG_CUBIC_HERMITE_CURVE_H
