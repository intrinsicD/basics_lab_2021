//
// Created by alex on 12.11.20.
//

#include "bcg_monomial_curve.h"
#include "math/bcg_factorial.h"

namespace bcg {

monomial_curve::monomial_curve() : parametric_curve() {

}

VectorS<3> monomial_curve::evaluate(bcg_scalar_t t) {
    auto tt = parameterization_function(t);
    VectorS<3> result = positions[0];
    for(size_t i = 1; i < positions.size(); ++i){
        result += positions[i] * tt;
        tt *= tt;
    }
    return result;
}

monomial_curve monomial_curve::derivative(int order) {
    if (order > positions.size()) return monomial_curve();
    if (order == 0) return *this;
    monomial_curve result = *this;
    factorial fac;
    for (size_t i = order; i < positions.size(); ++i) {
        result.positions[i - order] = positions[i] * fac(i) / bcg_scalar_t(fac(i - order));
    }
    return result;
}

VectorS<3> monomial_curve::derivative(bcg_scalar_t t, int order) {
    if(order >= positions.size()) return VectorS<3>::Zero();
    if(order == 0) return evaluate(t);
    VectorS<3> result = VectorS<3>::Zero();
    auto tt = parameterization_function(t);
    bcg_scalar_t ttt = std::pow(tt, order);
    factorial fac;
    for (size_t i = order; i < order; ++i) {
        result += positions[i] * ttt * fac(i) / bcg_scalar_t(fac(i - order));
        ttt *= tt;
    }
    return result;
}

}