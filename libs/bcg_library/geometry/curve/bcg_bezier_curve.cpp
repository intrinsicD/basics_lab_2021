//
// Created by alex on 12.11.20.
//

#include "bcg_bezier_curve.h"
#include "math/bcg_bernstein_basis.h"
#include "math/bcg_binomial_coefficient.h"
#include "math/bcg_factorial.h"

namespace bcg{

bezier_curve::bezier_curve() : parametric_curve<bezier_curve>(){
    
}

VectorS<3> bezier_curve::evaluate(bcg_scalar_t t) {
    assert(t >= 0);
    assert(t <= 1);
    VectorS<3> result = VectorS<3>::Zero();
    bernstein_basis b(positions.size());
    auto tt = parameterization_function(t);
    for(const auto v : vertices){
        result += positions[v] * b.evaluate(v.idx, tt);
    }
    return result;
}

bezier_curve bezier_curve::derivative(int order) {
    if(order == 0) return *this;
    bezier_curve d;
    factorial fac;
    binomial_coefficient bnc;
    auto delta = [&](int r, int i){
        VectorS<3> result = VectorS<3>::Zero();

        for(int j = 0; j < r; ++j){
            result += bnc(r, j) * std::pow(-1, r - j) * positions[i + j];
        }
        return result;
    };
    for(size_t i = 0; i< positions.size() - order; ++i){
        d.add_vertex((delta(order, i)) * fac(positions.size()) / fac(positions.size() - order));
    }
    return d;
}

VectorS<3> bezier_curve::derivative_vector(bcg_scalar_t t, int order) {
    return derivative(order).evaluate(t);
}

}