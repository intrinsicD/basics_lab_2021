//
// Created by alex on 12.11.20.
//

#include <gtest/gtest.h>
#include "math/bcg_polynomial.h"

using namespace bcg;

TEST(Polynomial, polynomial) {
    polynomial p;
    EXPECT_EQ(p.order, 0);
    EXPECT_TRUE(p.coefficients.empty());
}

TEST(Polynomial, polynomial_order) {
    polynomial p(5);
    EXPECT_EQ(p.order, 5);
    EXPECT_EQ(p.coefficients.size() , 5);
}

TEST(Polynomial, polynomial_coefficients) {
    polynomial p({1, 1, 1, 1, 1, 1});
    EXPECT_EQ(p.order, 6);
    EXPECT_EQ(p.coefficients.size() , 6);
    EXPECT_EQ(p.evaluate(0), 1);
    EXPECT_EQ(p.evaluate(1), 6);
    EXPECT_EQ(p.derivative(0).order, 6);
    EXPECT_EQ(p.derivative(1).order, 5);
    EXPECT_EQ(p.derivative(2).order, 4);
    EXPECT_EQ(p.derivative(3).order, 3);
    EXPECT_EQ(p.derivative(4).order, 2);
    EXPECT_EQ(p.derivative(5).order, 1);
    EXPECT_EQ(p.derivative(6).order, 0);
    EXPECT_EQ(p.derivative(0).coefficients, std::vector<bcg_scalar_t>({1, 1, 1, 1, 1, 1}));
    EXPECT_EQ(p.derivative(1).coefficients, std::vector<bcg_scalar_t>({1, 2, 3, 4, 5}));
    EXPECT_EQ(p.derivative(2).coefficients, std::vector<bcg_scalar_t>({2, 6, 12, 20}));
    EXPECT_EQ(p.derivative(3).coefficients, std::vector<bcg_scalar_t>({6, 24, 60}));
    EXPECT_EQ(p.derivative(4).coefficients, std::vector<bcg_scalar_t>({24, 120}));
    EXPECT_EQ(p.derivative(5).coefficients, std::vector<bcg_scalar_t>({120}));
    EXPECT_EQ(p.derivative(6).coefficients, std::vector<bcg_scalar_t>({}));
    EXPECT_EQ(p.derivative(0).evaluate(1), 6);
    EXPECT_EQ(p.derivative(1).evaluate(1), 15);
    EXPECT_EQ(p.derivative(2).evaluate(1), 40);
    EXPECT_EQ(p.derivative(3).evaluate(1), 90);
    EXPECT_EQ(p.derivative(4).evaluate(1), 144);
    EXPECT_EQ(p.derivative(5).evaluate(1), 120);
    EXPECT_EQ(p.derivative(6).evaluate(1), 0);
    EXPECT_EQ(p.derivative(0).evaluate(1), p.derivative_value(1, 0));
    EXPECT_EQ(p.derivative(1).evaluate(1), p.derivative_value(1, 1));
    EXPECT_EQ(p.derivative(2).evaluate(1), p.derivative_value(1, 2));
    EXPECT_EQ(p.derivative(3).evaluate(1), p.derivative_value(1, 3));
    EXPECT_EQ(p.derivative(4).evaluate(1), p.derivative_value(1, 4));
    EXPECT_EQ(p.derivative(5).evaluate(1), p.derivative_value(1, 5));
    EXPECT_EQ(p.derivative(6).evaluate(1), p.derivative_value(1, 6));
}

TEST(Polynomial, polynomial_vectorspace) {
    polynomial first({1, 1, 1, 1});
    polynomial second({2, 2, 2, 2});
    EXPECT_EQ(first + second, polynomial({3, 3, 3, 3}));
    EXPECT_EQ(second - first, first);
    EXPECT_EQ(first * 2, second);
    EXPECT_EQ(2 * first, second);
    EXPECT_EQ(second / 2, first);
}