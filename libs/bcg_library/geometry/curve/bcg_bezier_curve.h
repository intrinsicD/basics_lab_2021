//
// Created by alex on 12.11.20.
//

#ifndef BCG_GRAPHICS_BCG_BEZIER_CURVE_H
#define BCG_GRAPHICS_BCG_BEZIER_CURVE_H

#include "bcg_parametric_curve.h"

namespace bcg{

struct bezier_curve : public parametric_curve<bezier_curve>{
    bezier_curve();

    VectorS<3> evaluate(bcg_scalar_t t) override;

    bezier_curve derivative(int order) override;

    VectorS<3> derivative_vector(bcg_scalar_t t, int order) override;
};

}

#endif //BCG_GRAPHICS_BCG_BEZIER_CURVE_H
