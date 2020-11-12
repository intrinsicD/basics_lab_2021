//
// Created by alex on 12.11.20.
//

#include "bcg_bernstein_basis.h"
#include "math/bcg_binomial_coefficient.h"

namespace bcg{

bernstein_basis::bernstein_basis(){

}

bernstein_basis::bernstein_basis(int degree) : degree(degree){

}

bcg_scalar_t bernstein_basis::evaluate(int i, bcg_scalar_t t){
    if(i < 0 || degree < 0) return 0;
    binomial_coefficient bnc;
    return bnc(degree, i) * std::pow(t, i) * std::pow(1 - t, degree - i);
}

bcg_scalar_t bernstein_basis::derivative(int i, bcg_scalar_t t){
    return degree * (bernstein_basis(degree - 1).evaluate( i - 1, t) - bernstein_basis(degree - 1).evaluate(i, t));
}

}