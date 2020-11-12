//
// Created by alex on 12.11.20.
//


#include <gtest/gtest.h>
#include "math/bcg_monomial_basis.h"

using namespace bcg;

TEST(Monomial, evaluate) {
    EXPECT_EQ(monomial_basis::evaluate(0, 6), std::vector<bcg_scalar_t>({1, 0, 0, 0, 0, 0}));
    EXPECT_EQ(monomial_basis::evaluate(1, 6), std::vector<bcg_scalar_t>({1, 1, 1, 1, 1, 1}));
    int degree = 6;
    bcg_scalar_t t = 0.5;
    auto mb = monomial_basis::evaluate(t, degree);
    for(size_t i = 0; i < degree; ++i){
        EXPECT_EQ(mb[i], std::pow(t, i));
    }
}

TEST(Monomial, derivative) {
    EXPECT_EQ(monomial_basis::derivative(1, 6, 0), std::vector<bcg_scalar_t>({1, 1, 1, 1, 1, 1}));
    EXPECT_EQ(monomial_basis::derivative(1, 6, 1), std::vector<bcg_scalar_t>({0, 1, 2, 3, 4, 5}));
    EXPECT_EQ(monomial_basis::derivative(1, 6, 2), std::vector<bcg_scalar_t>({0, 0, 2, 6, 12, 20}));
    EXPECT_EQ(monomial_basis::derivative(1, 6, 3), std::vector<bcg_scalar_t>({0, 0, 0, 6, 24, 60}));
    EXPECT_EQ(monomial_basis::derivative(1, 6, 4), std::vector<bcg_scalar_t>({0, 0, 0, 0, 24, 120}));
    EXPECT_EQ(monomial_basis::derivative(1, 6, 5), std::vector<bcg_scalar_t>({0, 0, 0, 0, 0, 120}));
    EXPECT_EQ(monomial_basis::derivative(1, 6, 6), std::vector<bcg_scalar_t>({0, 0, 0, 0, 0, 0}));
}


TEST(Monomial, integrate) {
    EXPECT_EQ(monomial_basis::integrate(1, 6, 0), std::vector<bcg_scalar_t>({1, 1, 1, 1, 1, 1}));
    EXPECT_EQ(monomial_basis::integrate(1, 6, 1), std::vector<bcg_scalar_t>({1, 1, 1.0 / 2.0, 1.0 / 3.0, 1.0 / 4.0, 1.0 / 5.0, 1.0 / 6.0}));
    EXPECT_EQ(monomial_basis::integrate(1, 6, 2), std::vector<bcg_scalar_t>({1, 1, 1.0 / 2.0, 1.0 / (2.0 * 3.0), 1.0 / (3.0 * 4.0), 1.0 / (4.0 * 5.0), 1.0 / (5.0 * 6.0), 1.0 / (6.0 * 7.0)}));
    EXPECT_EQ(monomial_basis::integrate(1, 6, 3), std::vector<bcg_scalar_t>({1, 1, 1.0 / 2.0, 1.0 / (2.0 * 3.0), 1.0 / (2.0 * 3.0 * 4.0), 1.0 / (3.0 * 4.0 * 5.0), 1.0 / (4.0 * 5.0 * 6.0), 1.0 / (5.0 * 6.0 * 7.0), 1.0 / (6.0 * 7.0 * 8.0)}));
}