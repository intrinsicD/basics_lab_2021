//
// Created by alex on 13.11.20.
//

#ifndef BCG_GRAPHICS_BCG_CURVE_BEZIER_H
#define BCG_GRAPHICS_BCG_CURVE_BEZIER_H

#include "bcg_curve.h"

namespace bcg {

struct curve_bezier : curve {
    curve_bezier();

    explicit curve_bezier(const std::vector<VectorS<3>> &control_points);

    VectorS<3> evaluate(bcg_scalar_t t) const override;

    curve_bezier derivative(int order) const;

    VectorS<3> derivative_vector(bcg_scalar_t t, int order) const override;

    VectorS<3> de_casteljau(bcg_scalar_t t);

    std::array<curve_bezier, 2> subdivide(bcg_scalar_t t = 0.5);

    curve_bezier increase_degree();
};

}

#endif //BCG_GRAPHICS_BCG_CURVE_BEZIER_H
