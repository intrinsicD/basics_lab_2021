//
// Created by alex on 15.10.20.
//

#include <gtest/gtest.h>

#include "math/Mathematics/MapToEigen.h"
#include "math/bcg_math_common.h"

using namespace bcg;

TEST(TestVectorMap, vectorMap) {
    Vector<3, bcg_scalar_t> v;
    v.MakeUnit(1);

    auto test = Map(v);
    for (int j = 0; j < 3; ++j) {
        EXPECT_EQ(v[j], test(j));
    }
}

TEST(TestVectorMap, matrixMap) {
    Matrix<2, 3, bcg_scalar_t> m;
    m.MakeIdentity();
    auto test = Map(m);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            EXPECT_EQ(m(i, j), test(i, j));
        }
    }
}