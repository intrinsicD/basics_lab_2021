//
// Created by alex on 13.11.20.
//

#ifndef BCG_GRAPHICS_BCG_CURVE_POLYNOMIAL_H
#define BCG_GRAPHICS_BCG_CURVE_POLYNOMIAL_H

#include <array>
#include "bcg_curve.h"
#include "math/bcg_polynomial.h"

namespace bcg {

template<int N>
struct curve_polynomial : public curve<N> {
    curve_polynomial() : polynomials() {

    }

    curve_polynomial(const std::array<polynomial, N> &polynomials) : polynomials(polynomials) {

    }

    VectorS<N> evaluate(bcg_scalar_t t) const override {
        VectorS<N> result = zeros<N>;
        for (size_t i = 0; i < N; ++i) {
            result[N] = polynomials[i].evaluate(t);
        }
        return result;
    }


    VectorS<N> derivative_vector(bcg_scalar_t t, int order) const override {
        VectorS<N> result = zeros<N>;
        for (size_t i = 0; i < N; ++i) {
            result[N] = polynomials[i].derivative_vector(t, order);
        }
        return result;
    }


    std::vector<VectorS<N>> control_points() const override {
        std::vector<VectorS<N>> c_points(N);
        for(size_t i = 0; i < N; ++i){
            for(size_t j = 0; j < N; ++j){
                c_points[i][j] = polynomials[j].coefficients[i];
            }
        }
        return c_points;
    }

    std::array<polynomial, N> polynomials;
};

using curve_polynomial2 = curve_polynomial<2>;

using curve_polynomial3 = curve_polynomial<3>;

}

#endif //BCG_GRAPHICS_BCG_CURVE_POLYNOMIAL_H
