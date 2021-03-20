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

    for (size_t i = 0; i < static_cast<size_t>(degree()); ++i) {
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

    size_t degree_minus_order = degree() - order;
    for (size_t i = 0; i < degree_minus_order; ++i) {
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
    size_t degree_minus_order = degree() - order;
    bernstein_basis b(degree_minus_order);
    for (size_t i = 0; i < degree_minus_order; ++i) {
        result += b.evaluate(i, t) * delta(order, i) * fac(degree()) / bcg_scalar_t(fac(degree() - order));
    }
    return result;
}

VectorS<3> curve_bezier::de_casteljau(bcg_scalar_t t){
    std::vector<VectorS<3>> Q = control_points();
    for(size_t k = 1; k < Q.size(); ++k){
        for(size_t i = 0; i < Q.size() - k; ++i){
            Q[i] = (1.0 - t) * Q[i] + t * Q[i + 1];
        }
    }
    return Q[0];
}

std::array<curve_bezier, 2> curve_bezier::subdivide(bcg_scalar_t t){
    std::array<curve_bezier, 2> segments;

    segments[0].add_vertex(control_points()[0]);
    segments[1].add_vertex(control_points().back());
    std::vector<VectorS<3>> Q = control_points();
    for(size_t k = 1; k < Q.size(); ++k){
        for(size_t i = 0; i < Q.size() - k; ++i){
            Q[i] = (1.0 - t) * Q[i] + t * Q[i + 1];
            if(i == 0) segments[0].add_vertex(Q[i]);
            if(i == Q.size() - k + 1) segments[1].add_vertex(Q[i]);
        }
    }
    std::reverse(segments[1].positions.begin(),segments[1].positions.end());
    return segments;
}

curve_bezier curve_bezier::increase_degree(){
    curve_bezier higher_order;
    higher_order.add_vertex(control_points()[0]);
    size_t n = control_points().size() - 1;
    for(size_t i = 1; i < n + 1; ++i){
        bcg_scalar_t u = bcg_scalar_t(i) / bcg_scalar_t(n + 1);
        higher_order.add_vertex( u * control_points()[i - 1] + (1 - u) * control_points()[i]);
    }
    higher_order.add_vertex(control_points().back());
    return higher_order;
}


}