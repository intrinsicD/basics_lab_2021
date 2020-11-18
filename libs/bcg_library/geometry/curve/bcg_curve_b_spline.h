//
// Created by alex on 18.11.20.
//

#ifndef BCG_GRAPHICS_BCG_CURVE_B_SPLINE_H
#define BCG_GRAPHICS_BCG_CURVE_B_SPLINE_H

#include "bcg_curve.h"

namespace bcg {

struct curve_b_spline : curve {
    enum KnotType{
        uniform,
        nonuniform
    }knot_type;

    curve_b_spline();

    explicit curve_b_spline(const std::vector<VectorS<3>> &control_points, KnotType knot_type);

    VectorS<3> evaluate(bcg_scalar_t t) const override;

    curve_b_spline derivative(int order) const;

    VectorS<3> derivative_vector(bcg_scalar_t t, int order) const override;

    VectorS<3> de_boor(bcg_scalar_t t);

    std::array<curve_b_spline, 2> subdivide(bcg_scalar_t t = 0.5);

    curve_b_spline increase_degree();

    std::vector<bcg_scalar_t> knots;
    size_t degree = 1;
};

}

#endif //BCG_GRAPHICS_BCG_CURVE_B_SPLINE_H
