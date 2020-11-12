//
// Created by alex on 12.11.20.
//

#include "bcg_binomial_coefficient.h"

namespace bcg {

factorial binomial_coefficient::fac = factorial();

binomial_coefficient::binomial_coefficient(unsigned int n, unsigned int k) {
    value(n, k);
}

binomial_coefficient::~binomial_coefficient() {
    clear();
}

unsigned int binomial_coefficient::operator()(int n, int k) {
    return value(n, k);
}

unsigned int binomial_coefficient::value(int n, int k) {
    return fac(n) / (fac(k) * fac(n - k));
}

void binomial_coefficient::clear() {
    fac.clear();
}

}