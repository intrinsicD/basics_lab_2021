//
// Created by alex on 18.11.20.
//

#include "bcg_curve_b_spline.h"

namespace bcg {

curve_b_spline::curve_b_spline() {

}

curve_b_spline::curve_b_spline(const std::vector<VectorS<3>> &control_points_, KnotType knot_type) {
    for (const auto &cp : control_points_) {
        add_vertex(cp);
    }
    for (size_t i = 0; i < control_points_.size() - 1; ++i) {
        add_edge(i, i + 1);
    }
    bcg_scalar_t u = 0;
    bcg_scalar_t du = 1.0 / control_points_.size();
    if (knot_type == KnotType::nonuniform) {
        knots = {0, 0, 0, 0};
    }
    for (size_t i = 0; i < control_points_.size(); ++i) {
        knots.push_back(u);
        u += du;
    }
    if (knot_type == KnotType::nonuniform) {
        knots.push_back(1.0);
        knots.push_back(1.0);
        knots.push_back(1.0);
        knots.push_back(1.0);
    }
}

VectorS<3> curve_b_spline::evaluate(bcg_scalar_t /*t*/) const {

}

curve_b_spline curve_b_spline::derivative(int /*order*/) const {

}

VectorS<3> curve_b_spline::derivative_vector(bcg_scalar_t /*t*/, int /*order*/) const {

}

VectorS<3> curve_b_spline::de_boor(bcg_scalar_t t) {
    size_t i = 0;
    if (knot_type == KnotType::uniform) {
        i = std::floor(t * knots.size());
    } else {
        i = std::floor(t * bcg_scalar_t (knots.size() - 2 * (degree + 1))) + degree + 1;
    }
    std::vector<VectorS<3>> d(degree + 1);
    for (size_t j = 0; j < degree + 1; ++j) {
        d[j] = control_points()[j + i - degree];
    }
    for (size_t r = 1; r < degree + 1; ++r) {
        for (size_t j = degree; j > r - 1; --j) {
            bcg_scalar_t alpha = (t - knots[j + i - degree]) / (knots[j + 1 + i - r] - knots[j + i - degree]);
            d[j] = (1.0 - alpha) * d[j - 1] + alpha * d[j];
        }
    }
    return d[degree];
}

std::array<curve_b_spline, 2> curve_b_spline::subdivide(bcg_scalar_t t) {

}

curve_b_spline curve_b_spline::increase_degree() {

}

}