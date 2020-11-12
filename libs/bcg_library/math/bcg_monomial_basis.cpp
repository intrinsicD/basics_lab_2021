//
// Created by alex on 12.11.20.
//

#include "bcg_monomial_basis.h"
#include "bcg_factorial.h"

namespace bcg {


std::vector<bcg_scalar_t> monomial_basis::evaluate(bcg_scalar_t t, int degree){
    std::vector<bcg_scalar_t> basis(degree, 0);
    bcg_scalar_t tt = 1;
    for(auto & v : basis){
        v = tt;
        tt *= t;
    }
    return basis;
}

std::vector<bcg_scalar_t> monomial_basis::derivative(bcg_scalar_t t, int degree, int order){
    std::vector<bcg_scalar_t> basis(degree, 0);
    bcg_scalar_t tt = 1;
    factorial fac;
    for(size_t i = order; i < basis.size(); ++i){
        basis[i] = tt * fac(i) / bcg_scalar_t(fac(i - order));
        tt *= t;
    }
    return basis;
}

std::vector<bcg_scalar_t> monomial_basis::integrate(bcg_scalar_t t, int degree, int order){
    if(order == 0) return evaluate(t, degree);
    std::vector<bcg_scalar_t> basis(degree + order, 0);
    bcg_scalar_t tt = 1;
    factorial fac;
    for(size_t i = 0; i < basis.size(); ++i){
        basis[i] = tt * fac(i - order) / bcg_scalar_t(fac(i));
        tt *= t;
    }
    return basis;
}


}