//
// Created by alex on 12.11.20.
//

#include "bcg_polynomial.h"

#include <utility>
#include "math/bcg_vector_map_eigen.h"
#include "math/bcg_factorial.h"

namespace bcg {

polynomial::polynomial() : order(0) {

}

polynomial::polynomial(int order) : order(order), coefficients(order, 0) {

}

polynomial::polynomial(std::vector<bcg_scalar_t> coefficients) : order(static_cast<int>(coefficients.size())),
                                                                 coefficients(std::move(coefficients)) {

}

bcg_scalar_t polynomial::evaluate(bcg_scalar_t t) {
    if(coefficients.empty()) return 0;
    int i = static_cast<int>(coefficients.size());
    bcg_scalar_t result = coefficients[--i];
    for (--i; i >= 0; --i) {
        result *= t;
        result += coefficients[i];
    }
    return result;
}

bcg_scalar_t polynomial::derivative_value(bcg_scalar_t t, int degree) {
    if(degree >= order) return 0;
    if(degree == 0) return evaluate(t);
    bcg_scalar_t result = 0;
    bcg_scalar_t tt = std::pow(t, degree);
    factorial fac;
    for (size_t i = degree; i < order; ++i) {
        result += coefficients[i] * tt * fac(i) / bcg_scalar_t(fac(i - degree));
        tt *= t;
    }
    return result;
}

polynomial polynomial::derivative(int degree) {
    if (degree > order) return polynomial();
    if (degree == 0) return *this;
    polynomial result(order - degree);
    factorial fac;
    for (size_t i = degree; i < order; ++i) {
        result.coefficients[i - degree] = coefficients[i] * fac(i) / bcg_scalar_t(fac(i - degree));
    }
    return result;
}

bool polynomial::operator==(const polynomial &other) const {
    if (order != other.order) return false;
    for (size_t i = 0; i < order; ++i) {
        if (coefficients[i] != other.coefficients[i]) return false;
    }
    return true;
}

bool polynomial::operator!=(const polynomial &other) const {
    if (order != other.order) return true;
    for (size_t i = 0; i < order; ++i) {
        if (coefficients[i] != other.coefficients[i]) return true;
    }
    return false;
}

std::ostream &operator<<(std::ostream &stream, const polynomial &p) {
    stream << "order: " << p.order << "\n";
    stream << MapConst(p.coefficients).transpose() << "\n";
    return stream;
}


polynomial operator+(const polynomial &first, const polynomial &second) {
    int order = std::min(first.order, second.order);
    polynomial result(order);
    for (size_t i = 0; i < order; ++i) {
        result.coefficients[i] = first.coefficients[i] + second.coefficients[i];
    }
    return result;
}

polynomial operator-(const polynomial &first, const polynomial &second) {
    int order = std::min(first.order, second.order);
    polynomial result(order);
    for (size_t i = 0; i < order; ++i) {
        result.coefficients[i] = first.coefficients[i] - second.coefficients[i];
    }
    return result;
}

polynomial operator*(bcg_scalar_t s, const polynomial &p) {
    polynomial result(p.order);
    for (size_t i = 0; i < p.order; ++i) {
        result.coefficients[i] = p.coefficients[i] * s;
    }
    return result;
}

polynomial operator*(const polynomial &p, bcg_scalar_t s) {
    polynomial result(p.order);
    for (size_t i = 0; i < p.order; ++i) {
        result.coefficients[i] = p.coefficients[i] * s;
    }
    return result;
}

polynomial operator/(const polynomial &p, bcg_scalar_t s) {
    if (s == 0) return p;

    polynomial result(p.order);
    for (size_t i = 0; i < p.order; ++i) {
        result.coefficients[i] = p.coefficients[i] / s;
    }
    return result;
}

}