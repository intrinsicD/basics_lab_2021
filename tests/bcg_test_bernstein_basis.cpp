//
// Created by alex on 12.11.20.
//

#include <gtest/gtest.h>
#include "math/bcg_bernstein_basis.h"

using namespace bcg;

TEST(BernsteinBasis, polynomial) {
    int degree = 4;
    bernstein_basis p(degree);
    bcg_scalar_t t = 0;
    bcg_scalar_t dt = 0.01;

    for(size_t i = 0; i < 100; ++i){
        for(size_t j = 0; j < degree; ++j){
            EXPECT_GE(p.evaluate(j, t), 0);
            EXPECT_LE(std::abs(p.evaluate(degree - j, 1 - t) - p.evaluate(j, t)), 1e-5f);
        }
        t += dt;
    }
    EXPECT_FLOAT_EQ(p.evaluate(-1, 0.5), 0);
    p = bernstein_basis(-5);
    EXPECT_FLOAT_EQ(p.evaluate(1, 0.5), 0);
    EXPECT_FLOAT_EQ(p.evaluate(-1, 0.5), 0);
}
