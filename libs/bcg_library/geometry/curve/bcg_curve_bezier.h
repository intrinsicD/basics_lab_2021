//
// Created by alex on 13.11.20.
//

#ifndef BCG_GRAPHICS_BCG_CURVE_BEZIER_H
#define BCG_GRAPHICS_BCG_CURVE_BEZIER_H

#include "bcg_curve.h"
#include "math/bcg_bernstein_basis.h"
#include "math/bcg_binomial_coefficient.h"
#include "math/bcg_factorial.h"

namespace bcg{

template<int N>
struct curve_bezier : curve<N>{
    curve_bezier() : m_contol_points() {

    }

    curve_bezier(std::vector<VectorS<N>> contol_points) : m_contol_points(contol_points) {

    }

    VectorS<N> evaluate(bcg_scalar_t t) const override {
        VectorS<N> result = zeros<N>;
        assert(t >= 0);
        assert(t <= 1);
        bernstein_basis b(degree());
        for(size_t i = 0; i < degree(); ++i){
            result += m_contol_points[i] * b.evaluate(i, t);
        }
        return result;
    }

    curve_bezier derivative(int order) const {
        if(order == 0) return *this;
        curve_bezier d;
        factorial fac;
        binomial_coefficient bnc;
        auto delta = [&](int r, int i){
            VectorS<3> result = VectorS<3>::Zero();

            for(int j = 0; j < r; ++j){
                result += bnc(r, j) * std::pow(-1, r - j) * m_contol_points[i + j];
            }
            return result;
        };
        for(size_t i = 0; i< degree() - order; ++i){
            d.m_contol_points[i] = ((delta(order, i)) * fac(degree()) / fac(degree() - order));
        }
        return d;
    }


    VectorS<N> derivative_vector(bcg_scalar_t t, int order) const override {
        if(order == 0) return *this;
        binomial_coefficient bnc;
        auto delta = [&](int r, int i){
            VectorS<3> result = VectorS<3>::Zero();

            for(int j = 0; j < r; ++j){
                result += bnc(r, j) * std::pow(-1, r - j) * m_contol_points[i + j];
            }
            return result;
        };
        VectorS<N> result;
        factorial fac;
        bernstein_basis b(degree() - order);
        for(size_t i = 0; i < degree() - order; ++i){
            result[i] = ((delta(order, i)) * fac(degree()) / fac(degree() - order)) * b.evaluate(i, t);
        }
        return result;
    }

    std::vector<VectorS<N>> control_points() const override {
        return m_contol_points;
    }

    size_t degree() const {
        return m_contol_points.size();
    }

    std::vector<VectorS<N>> m_contol_points;
};

using curve_bezier2 = curve_bezier<2>;

using curve_bezier3 = curve_bezier<3>;

}

#endif //BCG_GRAPHICS_BCG_CURVE_BEZIER_H
