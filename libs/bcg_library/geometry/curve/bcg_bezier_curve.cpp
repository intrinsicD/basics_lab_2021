//
// Created by alex on 13.11.20.
//

#include "bcg_curve_bezier.h"
#include "math/bcg_bernstein_basis.h"
#include "math/bcg_binomial_coefficient.h"
#include "math/bcg_factorial.h"

namespace bcg {

curve_bezier::curve_bezier() : curve() {

}

curve_bezier::curve_bezier(const std::vector<VectorS<3>> &control_points_) {
    for (const auto &cp : control_points_) {
        add_vertex(cp);
    }
    for (size_t i = 0; i < control_points_.size() - 1; ++i) {
        add_edge(i, i + 1);
    }
}

VectorS<3> curve_bezier::evaluate(bcg_scalar_t t) const {
    VectorS<3> result = zeros<3>;
    assert(t >= 0);
    assert(t <= 1);
    bernstein_basis b(degree());
    for (size_t i = 0; i < degree(); ++i) {
        result += positions[i] * b.evaluate(i, t);
    }
    return result;
}

curve_bezier curve_bezier::derivative(int order) const {
    if (order == 0) return *this;
    curve_bezier d;
    factorial fac;
    binomial_coefficient bnc;
    auto delta = [&](int r, int i) {
        VectorS<3> result = VectorS<3>::Zero();

        for (int j = 0; j < r; ++j) {
            result += bnc(r, j) * std::pow(-1, r - j) * positions[i + j];
        }
        return result;
    };
    for (size_t i = 0; i < degree() - order; ++i) {
        d.add_vertex(delta(order, i) * fac(degree()) / bcg_scalar_t(fac(degree() - order)));
    }
    return d;
}


VectorS<3> curve_bezier::derivative_vector(bcg_scalar_t t, int order) const {
    if (order == 0) return evaluate(t);
    binomial_coefficient bnc;
    auto delta = [&](int r, int i) {
        VectorS<3> result = VectorS<3>::Zero();

        for (int j = 0; j < r; ++j) {
            result += bnc(r, j) * std::pow(-1, r - j) * positions[i + j];
        }
        return result;
    };
    VectorS<3> result = VectorS<3>::Zero();
    factorial fac;
    bernstein_basis b(degree() - order);
    for (size_t i = 0; i < degree() - order; ++i) {
        result += b.evaluate(i, t) * delta(order, i) * fac(degree()) / bcg_scalar_t(fac(degree() - order));
    }
    return result;
}


}