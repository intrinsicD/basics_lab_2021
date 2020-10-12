//
// Created by alex on 12.10.20.
//

#include <gtest/gtest.h>
#include "bcg_library/math/bcg_vector_t.h"

using namespace bcg;

class TestVectorFixture : public ::testing::Test {
public:
    TestVectorFixture() {
        vec1 = vector<5, float>(1.5);
        vec2 = vector<6, float>(vec1, 1.5);
        vec3 = {1, 2, 3, 4, 5, 6, 7};
        vec3_neg = {-1, -2, -3, -4, -5, -6, -7};
        zeros = vector<7, float>::zero();
        ones = vector<7, float>::one();
        infinities = vector<7, float>::infinity();
    }

    vector<10, float> vec;
    vector<5, float> vec1;
    vector<6, float> vec2;
    vector<7, float> vec3;
    vector<7, float> vec3_neg;
    vector<7, float> zeros;
    vector<7, float> ones;
    vector<7, float> infinities;
};

TEST_F(TestVectorFixture, constructors) {
    for (const auto &v : vec) {
        EXPECT_FLOAT_EQ(v, 0);
    }
    for (const auto &v : vec1) {
        EXPECT_FLOAT_EQ(v, 1.5);
    }
    for (const auto &v : vec2) {
        EXPECT_FLOAT_EQ(v, 1.5);
    }
    for (size_t i = 0; i < vec3.size(); ++i) {
        EXPECT_EQ(vec3[i], i + 1);
    }
}

TEST_F(TestVectorFixture, size) {
    EXPECT_EQ(vec.size(), 10);
    EXPECT_EQ(vec1.size(), 5);
    EXPECT_EQ(vec2.size(), 6);
    EXPECT_EQ(vec3.size(), 7);
}

TEST_F(TestVectorFixture, copy) {
    vec3.copy(zeros);
    EXPECT_EQ(vec3, zeros);

    vec3.copy(ones);
    EXPECT_EQ(vec3, ones);

    vec3.copy(infinities);
    EXPECT_EQ(vec3, infinities);
}

TEST_F(TestVectorFixture, sign) {
    EXPECT_EQ(vec3_neg, -vec3);
}


TEST_F(TestVectorFixture, operators) {
    vector<7, float> vec3_result = {1, 2, 3, 4, 5, 6, 7};
    vec3 += 3;
    vec3 -= 3;
    EXPECT_EQ(vec3_result, vec3);

    vec3 *= 3;
    vec3 /= 3;
    EXPECT_EQ(vec3_result, vec3);

    EXPECT_EQ(vec3 + vec3_neg, zeros);
    EXPECT_EQ(vec3 - vec3_neg, vec3 * 2);
    EXPECT_EQ(vec3 * ones, vec3);
    EXPECT_EQ(vec3 / ones, vec3);
    EXPECT_EQ(vec3 / vec3_neg, -ones);
}

TEST_F(TestVectorFixture, lift_project) {
    auto result = vec3.lift();
    EXPECT_EQ(result.back(), 1);
    vec3 = result.project();
    EXPECT_EQ(vec3.back(), 7);
}

TEST_F(TestVectorFixture, dot) {
    EXPECT_FLOAT_EQ(vec.dot(vec), 0);
    EXPECT_EQ(ones.dot(ones), ones.size());
    EXPECT_FLOAT_EQ(ones.dot(vec3), vec3.sum());
    EXPECT_FLOAT_EQ(vec3.dot(vec3), 140.0);
}

TEST_F(TestVectorFixture, squared_norm) {
    EXPECT_FLOAT_EQ(vec3.squared_norm(), 140.0);
}

TEST_F(TestVectorFixture, norm) {
    EXPECT_FLOAT_EQ(vec.norm(), 0);
    EXPECT_FLOAT_EQ(vec3.norm(), 11.832159566199232);
}

TEST_F(TestVectorFixture, squared_distance) {
    EXPECT_FLOAT_EQ(zeros.squared_distance(vec3), vec3.squared_norm());
}

TEST_F(TestVectorFixture, distance) {
    EXPECT_FLOAT_EQ(zeros.distance(vec3), vec3.norm());
}

TEST_F(TestVectorFixture, cos) {
    EXPECT_FLOAT_EQ(ones.cos(vec3), 0.8944271909999159);
}

TEST_F(TestVectorFixture, angle) {
    EXPECT_FLOAT_EQ(ones.angle(vec3), 0.46364760900080615);
}

TEST_F(TestVectorFixture, sin) {
    EXPECT_FLOAT_EQ(ones.sin(vec3), 0.447213595499958);
}

TEST_F(TestVectorFixture, minCoeff) {
    size_t index;
    vec3.min_coeff(index);
    EXPECT_EQ(index, 0);
}

TEST_F(TestVectorFixture, maxCoeff) {
    size_t index;
    vec3.max_coeff(index);
    EXPECT_EQ(index, vec3.size() - 1);
}

TEST_F(TestVectorFixture, mean) {
    EXPECT_FLOAT_EQ(vec3.mean(), 4.0);
}

TEST_F(TestVectorFixture, sum) {
    EXPECT_FLOAT_EQ(vec3.sum(), 28.0);
}

TEST_F(TestVectorFixture, normalize) {
    vec3.normalize();
    EXPECT_FLOAT_EQ(vec3.norm(), 1.0);
}

TEST_F(TestVectorFixture, orthogonal) {
    EXPECT_FLOAT_EQ(vec3.dot(vec3.orthogonal()), 0.0);
}

TEST_F(TestVectorFixture, orthonormal) {
    EXPECT_FLOAT_EQ(vec3.orthonormal().norm(), 1.0);
}

TEST_F(TestVectorFixture, orthonormalize) {
    EXPECT_LE(std::abs(vec3.orthonormalize(ones).dot(ones)), 1e-5f);
}

TEST_F(TestVectorFixture, abs) {
    EXPECT_EQ(vec3_neg.abs(), vec3);
}

TEST_F(TestVectorFixture, sqrt) {
    EXPECT_FLOAT_EQ(vec3.sqrt().square().norm(), vec3.norm());
}

TEST_F(TestVectorFixture, square) {
    EXPECT_FLOAT_EQ(vec3.square().sqrt().norm(), vec3.norm());
}

TEST_F(TestVectorFixture, exp) {
    EXPECT_FLOAT_EQ(vec3.exp().log().norm(), vec3.norm());
}

TEST_F(TestVectorFixture, log) {
    EXPECT_FLOAT_EQ(vec3.log().exp().norm(), vec3.norm());
}

TEST_F(TestVectorFixture, pow) {
    EXPECT_FLOAT_EQ(vec3.pow(3).pow(1 / 3.0).norm(), vec3.norm());
}

TEST_F(TestVectorFixture, clamped) {
    EXPECT_EQ(vec3.clamped(0, 1), ones);
}

TEST_F(TestVectorFixture, lerp) {
    EXPECT_EQ(zeros.lerp(ones, 0.5), ones / 2.0);
    EXPECT_EQ(zeros.lerp(vec3, ones), vec3);
}