//
// Created by alex on 12.11.20.
//

#ifndef BCG_GRAPHICS_BCG_BINOMIAL_COEFFICIENT_H
#define BCG_GRAPHICS_BCG_BINOMIAL_COEFFICIENT_H

#include "bcg_factorial.h"

namespace bcg{

struct binomial_coefficient{
    binomial_coefficient() = default;

    explicit binomial_coefficient(unsigned int n, unsigned int k);

    ~binomial_coefficient();

    unsigned int operator()(int n, int k) ;

    static unsigned int value(int n, int k);

    static void clear();
private:
    static factorial fac;
};

}

#endif //BCG_GRAPHICS_BCG_BINOMIAL_COEFFICIENT_H
