//
// Created by alex on 12.11.20.
//

#ifndef BCG_GRAPHICS_BCG_FACTORIAL_H
#define BCG_GRAPHICS_BCG_FACTORIAL_H

#include <vector>

namespace bcg{

struct factorial{
    factorial() = default;

    explicit factorial(unsigned int n);

    ~factorial();

    unsigned int operator()(int n) ;

    static unsigned int value(int n);

    static void clear();

private:
    static std::vector<unsigned int> cache;
};

}

#endif //BCG_GRAPHICS_BCG_FACTORIAL_H
