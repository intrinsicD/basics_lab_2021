//
// Created by alex on 25.05.21.
//

#include <gtest/gtest.h>
#include "geometry/grid/bcg_base_nd_grid.h"

using namespace bcg;

class TestBaseGridFixture : public ::testing::Test{
public:
    TestBaseGridFixture() : grid({3, 3, 3}, aligned_box3::unit()){

    }

    base_grid3 grid;
};

TEST_F(TestBaseGridFixture, Constructor) {
    EXPECT_FLOAT_EQ(grid.vsl(0), 2.0 / 3.0);
    EXPECT_FLOAT_EQ(grid.vsl(1), 2.0 / 3.0);
    EXPECT_FLOAT_EQ(grid.vsl(2), 2.0 / 3.0);
    EXPECT_FLOAT_EQ(grid.aabb_diagonal(0), 2.0);
    EXPECT_FLOAT_EQ(grid.aabb_diagonal(1), 2.0);
    EXPECT_FLOAT_EQ(grid.aabb_diagonal(2), 2.0);
}

TEST_F(TestBaseGridFixture, to_idx) {
    auto idx = grid.to_idx(-ones<3>);
    EXPECT_EQ(idx, 0);
    idx = grid.to_idx(ones<3>);
    EXPECT_EQ(idx, 26);
}

TEST_F(TestBaseGridFixture, to_coord) {
    VectorI<3> coord = grid.to_coord(-ones<3>);
    EXPECT_EQ(coord(0), 0);
    EXPECT_EQ(coord(1), 0);
    EXPECT_EQ(coord(2), 0);
    coord = grid.to_coord(ones<3>);
    EXPECT_EQ(coord(0), 2);
    EXPECT_EQ(coord(1), 2);
    EXPECT_EQ(coord(2), 2);
}

TEST_F(TestBaseGridFixture, to_voxel_center) {
    VectorS<3> center = grid.to_voxel_center(-ones<3>);
    EXPECT_FLOAT_EQ(center(0), -1.0 + 1.0 / 3.0);
    EXPECT_FLOAT_EQ(center(1), -1.0 + 1.0 / 3.0);
    EXPECT_FLOAT_EQ(center(2), -1.0 + 1.0 / 3.0);
    center = grid.to_voxel_center(ones<3>);
    EXPECT_FLOAT_EQ(center(0), 1.0 - 1.0 / 3.0);
    EXPECT_FLOAT_EQ(center(1), 1.0 - 1.0 / 3.0);
    EXPECT_FLOAT_EQ(center(2), 1.0 - 1.0 / 3.0);
}

TEST_F(TestBaseGridFixture, to_voxel_min) {
    VectorS<3> min = grid.to_voxel_min(-ones<3>);
    EXPECT_FLOAT_EQ(min(0), -1.0);
    EXPECT_FLOAT_EQ(min(1), -1.0);
    EXPECT_FLOAT_EQ(min(2), -1.0);
    min = grid.to_voxel_min(ones<3>);
    EXPECT_FLOAT_EQ(min(0), 1.0 - 2.0 / 3.0);
    EXPECT_FLOAT_EQ(min(1), 1.0 - 2.0 / 3.0);
    EXPECT_FLOAT_EQ(min(2), 1.0 - 2.0 / 3.0);
}

TEST_F(TestBaseGridFixture, to_voxel_max) {
    VectorS<3> max = grid.to_voxel_max(-ones<3>);
    EXPECT_FLOAT_EQ(max(0), -1.0 + 2.0 / 3.0);
    EXPECT_FLOAT_EQ(max(1), -1.0 + 2.0 / 3.0);
    EXPECT_FLOAT_EQ(max(2), -1.0 + 2.0 / 3.0);
    max = grid.to_voxel_max(ones<3>);
    EXPECT_FLOAT_EQ(max(0), 1.0);
    EXPECT_FLOAT_EQ(max(1), 1.0);
    EXPECT_FLOAT_EQ(max(2), 1.0);
}

TEST_F(TestBaseGridFixture, coord_to_idx) {
    auto idx = grid.coord_to_idx({0, 0, 0});
    EXPECT_EQ(idx, 0);
    idx = grid.coord_to_idx({2, 2, 2});
    EXPECT_EQ(idx, 26);
}

TEST_F(TestBaseGridFixture, coord_to_voxel_center) {
    VectorS<3> center = grid.coord_to_voxel_center({0, 0, 0});
    EXPECT_FLOAT_EQ(center(0), -1.0 + 1.0 / 3.0);
    EXPECT_FLOAT_EQ(center(1), -1.0 + 1.0 / 3.0);
    EXPECT_FLOAT_EQ(center(2), -1.0 + 1.0 / 3.0);
    center = grid.coord_to_voxel_center({2, 2, 2});
    EXPECT_FLOAT_EQ(center(0), 1.0 - 1.0 / 3.0);
    EXPECT_FLOAT_EQ(center(1), 1.0 - 1.0 / 3.0);
    EXPECT_FLOAT_EQ(center(2), 1.0 - 1.0 / 3.0);
}

TEST_F(TestBaseGridFixture, coord_to_voxel_min) {
    VectorS<3> min = grid.coord_to_voxel_min({0, 0, 0});
    EXPECT_FLOAT_EQ(min(0), -1.0);
    EXPECT_FLOAT_EQ(min(1), -1.0);
    EXPECT_FLOAT_EQ(min(2), -1.0);
    min = grid.coord_to_voxel_min({2, 2, 2});
    EXPECT_FLOAT_EQ(min(0), 1.0 - 2.0 / 3.0);
    EXPECT_FLOAT_EQ(min(1), 1.0 - 2.0 / 3.0);
    EXPECT_FLOAT_EQ(min(2), 1.0 - 2.0 / 3.0);
}

TEST_F(TestBaseGridFixture, coord_to_voxel_max) {
    VectorS<3> max = grid.coord_to_voxel_max({0, 0, 0});
    EXPECT_FLOAT_EQ(max(0), -1.0 + 2.0 / 3.0);
    EXPECT_FLOAT_EQ(max(1), -1.0 + 2.0 / 3.0);
    EXPECT_FLOAT_EQ(max(2), -1.0 + 2.0 / 3.0);
    max = grid.coord_to_voxel_max({2, 2, 2});
    EXPECT_FLOAT_EQ(max(0), 1.0);
    EXPECT_FLOAT_EQ(max(1), 1.0);
    EXPECT_FLOAT_EQ(max(2), 1.0);
}

TEST_F(TestBaseGridFixture, idx_to_coord) {
    VectorI<3> coord = grid.idx_to_coord(0);
    EXPECT_EQ(coord(0),0);
    EXPECT_EQ(coord(1),0);
    EXPECT_EQ(coord(2),0);
    coord = grid.idx_to_coord(26);
    EXPECT_EQ(coord(0),2);
    EXPECT_EQ(coord(1),2);
    EXPECT_EQ(coord(2),2);
}

TEST_F(TestBaseGridFixture, idx_to_voxel_center) {
    VectorS<3> center = grid.idx_to_voxel_center(0);
    EXPECT_FLOAT_EQ(center(0), -1.0 + 1.0 / 3.0);
    EXPECT_FLOAT_EQ(center(1), -1.0 + 1.0 / 3.0);
    EXPECT_FLOAT_EQ(center(2), -1.0 + 1.0 / 3.0);
    center = grid.idx_to_voxel_center(26);
    EXPECT_FLOAT_EQ(center(0), 1.0 - 1.0 / 3.0);
    EXPECT_FLOAT_EQ(center(1), 1.0 - 1.0 / 3.0);
    EXPECT_FLOAT_EQ(center(2), 1.0 - 1.0 / 3.0);
}

TEST_F(TestBaseGridFixture, idx_to_voxel_min) {
    VectorS<3> min = grid.idx_to_voxel_min(0);
    EXPECT_FLOAT_EQ(min(0), -1.0);
    EXPECT_FLOAT_EQ(min(1), -1.0);
    EXPECT_FLOAT_EQ(min(2), -1.0);
    min = grid.idx_to_voxel_min(26);
    EXPECT_FLOAT_EQ(min(0), 1.0 - 2.0 / 3.0);
    EXPECT_FLOAT_EQ(min(1), 1.0 - 2.0 / 3.0);
    EXPECT_FLOAT_EQ(min(2), 1.0 - 2.0 / 3.0);
}

TEST_F(TestBaseGridFixture, idx_to_voxel_max) {
    VectorS<3> max = grid.idx_to_voxel_max(0);
    EXPECT_FLOAT_EQ(max(0), -1.0 + 2.0 / 3.0);
    EXPECT_FLOAT_EQ(max(1), -1.0 + 2.0 / 3.0);
    EXPECT_FLOAT_EQ(max(2), -1.0 + 2.0 / 3.0);
    max = grid.idx_to_voxel_max(26);
    EXPECT_FLOAT_EQ(max(0), 1.0);
    EXPECT_FLOAT_EQ(max(1), 1.0);
    EXPECT_FLOAT_EQ(max(2), 1.0);
}

TEST_F(TestBaseGridFixture, capacity) {
    auto capacity = grid.capacity();
    EXPECT_EQ(capacity, 27);
    grid.set_dims({6, 6, 6});
    capacity = grid.capacity();
    EXPECT_EQ(capacity, 216);
}

TEST_F(TestBaseGridFixture, is_boundary) {
    auto is_boundary = grid.is_boundary({0, 0, 0});
    EXPECT_TRUE(is_boundary);
    is_boundary = grid.is_boundary({2, 2, 2});
    EXPECT_TRUE(is_boundary);
    is_boundary = grid.is_boundary({2, 2, 1});
    EXPECT_TRUE(is_boundary);
    is_boundary = grid.is_boundary({1, 1, 1});
    EXPECT_FALSE(is_boundary);
}

TEST_F(TestBaseGridFixture, boundary_coords) {
    auto boundary_coords = grid.boundary_coords();
    EXPECT_EQ(boundary_coords.size(), 26);
    for(const VectorI<3> &coord : boundary_coords){
        auto idx = grid.coord_to_idx(coord);
        if(idx == 13){
            EXPECT_FALSE(grid.is_boundary(coord));
        }else{
            EXPECT_TRUE(grid.is_boundary(coord));
        }
    }
}