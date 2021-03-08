//
// Created by alex on 15.10.20.
//

#include <gtest/gtest.h>

#include "geometry/triangle/bcg_triangle.h"
#include "geometry/triangle/bcg_triangle_centers.h"
#include "geometry/triangle/bcg_barycentric_coordinates.h"

using namespace bcg;

class TriangleTest : public ::testing::Test {
public:
    TriangleTest() : triangle(),
                     third3f{1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0} {
        c = centroid(triangle);
        n = normal(triangle);
    }

    triangle3 triangle;
    VectorS<3> third3f;
    VectorS<3> c;
    VectorS<3> n;
};

TEST_F(TriangleTest, BarycentricCoordinates) {
    VectorS<3> bc = to_barycentric_coordinates(triangle, triangle.points[0]);
    EXPECT_EQ(bc, VectorS<3>(1, 0, 0));
    EXPECT_EQ(bc.sum(), bcg_scalar_t(1.0));

    bc = to_barycentric_coordinates(triangle, triangle.points[1]);
    EXPECT_EQ(bc, VectorS<3>(0, 1, 0));
    EXPECT_EQ(bc.sum(), bcg_scalar_t(1.0));

    bc = to_barycentric_coordinates(triangle, triangle.points[2]);
    EXPECT_EQ(bc, VectorS<3>(0, 0, 1));
    EXPECT_FLOAT_EQ(bc.sum(),bcg_scalar_t(1.0));

    bc = to_barycentric_coordinates(triangle, c);
    EXPECT_LE((bc - third3f).norm(), scalar_eps);
    EXPECT_FLOAT_EQ(bc.sum(),bcg_scalar_t(1.0));

    bc = to_barycentric_coordinates(triangle, (c + n).eval());
    EXPECT_LE((bc - third3f).norm(), scalar_eps);
    EXPECT_FLOAT_EQ(bc.sum(),bcg_scalar_t(1.0));
}

TEST_F(TriangleTest, Normal) {
    EXPECT_FLOAT_EQ(n.norm(),bcg_scalar_t(1.0));
}

TEST_F(TriangleTest, Area) {
    EXPECT_FLOAT_EQ(triangle.area(), 0.5f);
}

TEST_F(TriangleTest, Contains) {
    EXPECT_TRUE(triangle.contains(c));
    EXPECT_TRUE(triangle.contains(c + n));

    EXPECT_FALSE(triangle.contains(-one3s));
}

TEST_F(TriangleTest, Edges) {
    EXPECT_EQ(triangle.edge0(), VectorS<3>(1, 0, 0));
    EXPECT_EQ(triangle.edge1(), VectorS<3>(-1, 1, 0));
    EXPECT_EQ(triangle.edge2(), VectorS<3>(0, -1, 0));
    VectorS<3> elengths = triangle.edge_lengths();
    EXPECT_FLOAT_EQ(elengths[0],bcg_scalar_t(1.0));
    EXPECT_FLOAT_EQ(elengths[1], std::sqrt(2.0f));
    EXPECT_FLOAT_EQ(elengths[2],bcg_scalar_t(1.0));
    EXPECT_FLOAT_EQ(triangle.perimeter(), 2 + std::sqrt(2.0f));
}

TEST_F(TriangleTest, PointFromBarycentricCoords) {
    EXPECT_EQ(from_barycentric_coords(triangle, VectorS<3>(1, 0, 0)), triangle.points[0]);
    EXPECT_EQ(from_barycentric_coords(triangle, VectorS<3>(0, 1, 0)), triangle.points[1]);
    EXPECT_EQ(from_barycentric_coords(triangle, VectorS<3>(0, 0, 1)), triangle.points[2]);
    EXPECT_EQ(from_barycentric_coords(triangle, third3f), c);
}

TEST_F(TriangleTest, centers) {
    auto center = incenter(triangle);
    EXPECT_LE((center - from_barycentric_coords(triangle, to_barycentric_coordinates(triangle, center))).norm(), scalar_eps);
    center = centroid(triangle);
    EXPECT_LE((center - from_barycentric_coords(triangle, to_barycentric_coordinates(triangle, center))).norm(), scalar_eps);
    center = circumcenter(triangle);
    EXPECT_LE((center - from_barycentric_coords(triangle, to_barycentric_coordinates(triangle, center))).norm(), scalar_eps);
    center = orthocenter(triangle);
    EXPECT_LE((center - from_barycentric_coords(triangle, to_barycentric_coordinates(triangle, center))).norm(), scalar_eps);
    center = nine_point_center(triangle);
    EXPECT_LE((center - from_barycentric_coords(triangle, to_barycentric_coordinates(triangle, center))).norm(), scalar_eps);
    center = symmedian_point(triangle);
    EXPECT_LE((center - from_barycentric_coords(triangle, to_barycentric_coordinates(triangle, center))).norm(), scalar_eps);
    center = gergonne_point(triangle);
    EXPECT_LE((center - from_barycentric_coords(triangle, to_barycentric_coordinates(triangle, center))).norm(), scalar_eps);
    center = nagel_point(triangle);
    EXPECT_LE((center - from_barycentric_coords(triangle, to_barycentric_coordinates(triangle, center))).norm(), scalar_eps);
    center = mitten_point(triangle);
    EXPECT_LE((center - from_barycentric_coords(triangle, to_barycentric_coordinates(triangle, center))).norm(), scalar_eps);
    center = spieker_point(triangle);
    EXPECT_LE((center - from_barycentric_coords(triangle, to_barycentric_coordinates(triangle, center))).norm(), scalar_eps);
}
