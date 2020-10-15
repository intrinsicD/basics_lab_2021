//
// Created by alex on 15.10.20.
//

#include <gtest/gtest.h>

#include "geometry/sphere/bcg_sphere.h"
#include "geometry/sphere/bcg_sphere_fit.h"
#include "geometry/sphere/bcg_sphere_sampling_fibunacci.h"

using namespace bcg;

class TestSphere : public ::testing::Test {
public:
    TestSphere() : sphere(VectorS<3>::Random(), 5),
                   sphere_fit_square_lengths(VectorS<3>::Random(), 3),
                   sphere_fit_lengths(VectorS<3>::Random(), 7) {

    }

    sphere3 sphere;
    sphere3 sphere_fit_square_lengths;
    sphere3 sphere_fit_lengths;
};

TEST_F(TestSphere, least_squares_fit) {
    std::vector<VectorS<3>> points = sphere_fibunacci_sampling(sphere, 100);

    FitUsingSquaredLengths(points.size(), points.data(), sphere_fit_square_lengths);
    FitUsingLengths(points.size(), points.data(),  100, false, sphere_fit_lengths);

    EXPECT_LE((sphere.center - sphere_fit_square_lengths.center).squaredNorm(), scalar_eps);
    EXPECT_FLOAT_EQ(sphere.radius, sphere_fit_square_lengths.radius);

    EXPECT_LE((sphere.center - sphere_fit_lengths.center).squaredNorm(), scalar_eps);
    EXPECT_FLOAT_EQ(sphere.radius, sphere_fit_lengths.radius);
}
