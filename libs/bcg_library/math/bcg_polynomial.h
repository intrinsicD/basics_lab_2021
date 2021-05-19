//
// Created by alex on 12.11.20.
//

#ifndef BCG_GRAPHICS_BCG_POLYNOMIAL_H
#define BCG_GRAPHICS_BCG_POLYNOMIAL_H

#include <vector>
#include <iostream>
#include "bcg_math_types.h"

namespace bcg {
struct polynomial {
    polynomial();

    polynomial(int order);

    explicit polynomial(std::vector<bcg_scalar_t> coefficients);

    bcg_scalar_t evaluate(bcg_scalar_t t);

    bcg_scalar_t derivative_value(bcg_scalar_t t, int degree);

    polynomial derivative(int degree);

    bool operator==(const polynomial &other) const;

    bool operator!=(const polynomial &other) const;

    int order;
    std::vector<bcg_scalar_t> coefficients;
};

std::ostream& operator<<(std::ostream &stream, const polynomial &p);

polynomial operator+(const polynomial &first, const polynomial &second);

polynomial operator-(const polynomial &first, const polynomial &second);

polynomial operator*(bcg_scalar_t s, const polynomial &p);

polynomial operator*(const polynomial &p, bcg_scalar_t s);

polynomial operator/(const polynomial &p, bcg_scalar_t s);

};


#endif //BCG_GRAPHICS_BCG_POLYNOMIAL_H
