//
// Created by Alex on 08.05.20.
//

#include <gtest/gtest.h>

#include "geometry/grid/bcg_occupancy_grid.h"

using namespace bcg;

TEST(TestOccupancyGrid, constructor) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    EXPECT_EQ(grid.capacity(), 125);
}

TEST(TestOccupancyGrid, has_add_remove) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    std::vector<VectorS<3>> items;
    for (size_t i = 0; i < 100; ++i) {
        items.push_back(VectorS<3>::Random());
        EXPECT_FALSE(grid.is_occupied_point(items.back()));
        grid.mark_occupied_point(items.back());
        EXPECT_TRUE(grid.is_occupied_point(items.back()));
        grid.mark_free_point(items.back());
    }

    for (const auto &item : items) {
        EXPECT_FALSE(grid.is_occupied_point(item));
    }

    auto coord = grid.to_coord(VectorS<3>({-1, -1, -1}));
    EXPECT_EQ(coord, VectorI<3>::Zero());
    EXPECT_EQ(grid.coord_to_idx(coord), 0);
    EXPECT_EQ(grid.voxel_side_length(), VectorS<3>::Constant(bcg_scalar_t(2.0) / 5));
    coord = grid.to_coord(VectorS<3>({-1 + 2.1f / 5, -1, -1}));
    EXPECT_EQ(coord, VectorI<3>({1, 0, 0}));
    EXPECT_EQ(grid.coord_to_idx(coord), 1);

    coord = grid.to_coord(VectorS<3>({-1, -1 + 2.01f / 5, -1}));
    EXPECT_EQ(coord, VectorI<3>({0, 1, 0}));
    EXPECT_EQ(grid.coord_to_idx(coord), 5);

    coord = grid.to_coord(VectorS<3>({-1, -1, -1 + 2.01f / 5}));
    EXPECT_EQ(coord, VectorI<3>({0, 0, 1}));
    EXPECT_EQ(grid.coord_to_idx(coord), 25);

    coord = grid.to_coord(VectorS<3>({-1 + 2.01f / 5, -1, -1 + 2.01f / 5}));
    EXPECT_EQ(coord, VectorI<3>({1, 0, 1}));
    EXPECT_EQ(grid.coord_to_idx(coord), 26);

    coord = grid.to_coord(VectorS<3>({1, 1, 1}));
    EXPECT_EQ(coord, VectorI<3>({4, 4, 4}));
    EXPECT_EQ(grid.coord_to_idx(coord), 124);
}

TEST(TestOccupancyGrid, to_idx) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    EXPECT_EQ(0, grid.to_idx(grid.aabb.min));
    EXPECT_EQ(62, grid.to_idx(grid.aabb.center()));
    EXPECT_EQ(124, grid.to_idx(grid.aabb.max));
}

TEST(TestOccupancyGrid, to_coord) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    EXPECT_EQ(VectorI<3>::Zero(), grid.to_coord(grid.aabb.min));
    EXPECT_EQ(VectorI<3>::Constant(1), grid.to_coord(grid.aabb.center() - grid.voxel_side_length()));
    EXPECT_EQ(VectorI<3>::Constant(2), grid.to_coord(grid.aabb.center() - grid.voxel_side_length() / 2));
    EXPECT_EQ(VectorI<3>::Constant(2), grid.to_coord(grid.aabb.center()));
    EXPECT_EQ(VectorI<3>::Constant(3), grid.to_coord(grid.aabb.center() + grid.voxel_side_length() / 2));
    EXPECT_EQ(VectorI<3>::Constant(3), grid.to_coord(grid.aabb.center() + grid.voxel_side_length()));
    EXPECT_EQ(VectorI<3>::Constant(4), grid.to_coord(grid.aabb.max));
}

TEST(TestOccupancyGrid, to_voxel_center) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());
    float eps = 1e-6f;
    EXPECT_LE((grid.aabb.center() - grid.to_voxel_center(grid.aabb.center())).norm(), eps);
}

TEST(TestOccupancyGrid, to_voxel_min) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    float eps = 1e-6f;
    EXPECT_LE(
            (grid.aabb.center() - grid.voxel_side_length() / 2 - grid.to_voxel_min(grid.aabb.center())).norm(),
            eps);
}

TEST(TestOccupancyGrid, to_voxel_max) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    float eps = 1e-6f;
    EXPECT_LE(
            (grid.aabb.center() + grid.voxel_side_length() / 2 - grid.to_voxel_max(grid.aabb.center())).norm(),
            eps);
}

TEST(TestOccupancyGrid, coord_to_idx) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            for (int k = 0; k < 5; ++k) {
                VectorI<3> coord(i, j, k);
                EXPECT_EQ(coord, grid.idx_to_coord(grid.coord_to_idx(coord)));
            }
        }
    }

    EXPECT_EQ(0, grid.coord_to_idx(VectorI<3>::Zero()));
    EXPECT_EQ(62, grid.coord_to_idx(VectorI<3>::Constant(2)));
    EXPECT_EQ(124, grid.coord_to_idx(VectorI<3>::Constant(4)));
}

TEST(TestOccupancyGrid, coord_to_voxel_center) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            for (int k = 0; k < 5; ++k) {
                VectorI<3> coord(i, j, k);
                EXPECT_EQ(coord, grid.to_coord(grid.coord_to_voxel_center(coord)));
            }
        }
    }

    float eps = 1e-6f;
    EXPECT_LE((grid.aabb.min + grid.voxel_side_length() / 2 - grid.coord_to_voxel_center(VectorI<3>::Zero())).norm(),
              eps);
    EXPECT_LE((grid.aabb.center() - grid.coord_to_voxel_center(VectorI<3>::Constant(2))).norm(), eps);
    EXPECT_LE((grid.aabb.max - grid.voxel_side_length() / 2 -
               grid.coord_to_voxel_center(VectorI<3>::Constant(4))).norm(), eps);
}

TEST(TestOccupancyGrid, coord_to_voxel_min) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    float eps = 1e-6f;
    EXPECT_LE((grid.aabb.center() - grid.voxel_side_length() / 2 -
               grid.coord_to_voxel_min(VectorI<3>::Constant(2))).norm(), eps);
}

TEST(TestOccupancyGrid, coord_to_voxel_max) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    float eps = 1e-6f;
    EXPECT_LE((grid.aabb.center() + grid.voxel_side_length() / 2 -
               grid.coord_to_voxel_max(VectorI<3>::Constant(2))).norm(), eps);
}

TEST(TestOccupancyGrid, idx_to_coord) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    for (size_t i = 0; i < grid.capacity(); ++i) {
        EXPECT_EQ(i, grid.coord_to_idx(grid.idx_to_coord(i)));
    }

    EXPECT_EQ(grid.idx_to_coord(0), VectorI<3>::Zero());
    EXPECT_EQ(grid.idx_to_coord(57), VectorI<3>(2, 1, 2));
    EXPECT_EQ(grid.idx_to_coord(61), VectorI<3>(1, 2, 2));
    EXPECT_EQ(grid.idx_to_coord(62), VectorI<3>::Constant(2));
    EXPECT_EQ(grid.idx_to_coord(63), VectorI<3>(3, 2, 2));
    EXPECT_EQ(grid.idx_to_coord(67), VectorI<3>(2, 3, 2));
    EXPECT_EQ(grid.idx_to_coord(124), VectorI<3>::Constant(4));
}

TEST(TestOccupancyGrid, idx_to_voxel_center) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    float eps = 1e-6f;
    EXPECT_LE((grid.aabb.center() - grid.idx_to_voxel_center(62)).norm(), eps);
}

TEST(TestOccupancyGrid, idx_to_voxel_min) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    float eps = 1e-6f;
    EXPECT_LE((grid.aabb.min + grid.voxel_side_length() / 2 - grid.idx_to_voxel_center(0)).norm(), eps);
}

TEST(TestOccupancyGrid, itd_to_voxel_max) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    float eps = 1e-6f;
    EXPECT_LE((grid.aabb.max - grid.voxel_side_length() / 2 - grid.idx_to_voxel_center(124)).norm(), eps);
}


TEST(TestOccupancyGrid, voxel_side_length) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    EXPECT_EQ(grid.voxel_side_length(),
              (grid.aabb.diagonal().array() / grid.dims.cast<bcg_scalar_t >().array()).matrix());
    EXPECT_EQ(grid.voxel_side_length() / 2,
              (grid.aabb.halfextent().array() / grid.dims.cast<bcg_scalar_t>().array()).matrix());
}

TEST(TestOccupancyGrid, traverse) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    size_t count = 0;
    bool has_any = false;
    traverse(grid, [&has_any, &count](size_t idx, occupancy_grid &grid) {
        ++count;
        has_any = grid.is_occupied_idx(idx);
        return true;
    });
    EXPECT_EQ(count, grid.capacity());
    EXPECT_FALSE(has_any);

    grid.mark_occupied_point(grid.aabb.min);
    grid.mark_occupied_point(grid.aabb.center());
    grid.mark_occupied_point(grid.aabb.max);

    count = 0;
    has_any = false;
    traverse(grid, [&has_any, &count](size_t idx, occupancy_grid &grid) {
        if (grid.is_occupied_idx(idx)) {
            ++count;
            has_any = grid.is_occupied_idx(idx);
        }
        return true;
    });
    EXPECT_EQ(count, 3);
    EXPECT_TRUE(has_any);

    grid.mark_free_point(grid.aabb.center());

    count = 0;
    has_any = false;
    traverse(grid, [&has_any, &count](size_t idx, occupancy_grid &grid) {
        if (grid.is_occupied_idx(idx)) {
            ++count;
            has_any = grid.is_occupied_idx(idx);
        }
        return true;
    });
    EXPECT_EQ(count, 2);
    EXPECT_TRUE(has_any);
}

TEST(TestOccupancyGrid, query_knn) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());

    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(1, 1, 1)));
    grid.mark_occupied_coord(VectorI<3>(1, 1, 1));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(1, 1, 2)));
    grid.mark_occupied_coord(VectorI<3>(1, 1, 2));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(1, 1, 3)));
    grid.mark_occupied_coord(VectorI<3>(1, 1, 3));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(1, 2, 1)));
    grid.mark_occupied_coord(VectorI<3>(1, 2, 1));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(1, 2, 2)));
    grid.mark_occupied_coord(VectorI<3>(1, 2, 2));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(1, 2, 3)));
    grid.mark_occupied_coord(VectorI<3>(1, 2, 3));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(1, 3, 1)));
    grid.mark_occupied_coord(VectorI<3>(1, 3, 1));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(1, 3, 2)));
    grid.mark_occupied_coord(VectorI<3>(1, 3, 2));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(1, 3, 3)));
    grid.mark_occupied_coord(VectorI<3>(1, 3, 3));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(2, 1, 1)));
    grid.mark_occupied_coord(VectorI<3>(2, 1, 1));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(2, 1, 2)));
    grid.mark_occupied_coord(VectorI<3>(2, 1, 2));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(2, 1, 3)));
    grid.mark_occupied_coord(VectorI<3>(2, 1, 3));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(2, 2, 1)));
    grid.mark_occupied_coord(VectorI<3>(2, 2, 1));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(2, 2, 2)));
    grid.mark_occupied_coord(VectorI<3>(2, 2, 2));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(2, 2, 3)));
    grid.mark_occupied_coord(VectorI<3>(2, 2, 3));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(2, 3, 1)));
    grid.mark_occupied_coord(VectorI<3>(2, 3, 1));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(2, 3, 2)));
    grid.mark_occupied_coord(VectorI<3>(2, 3, 2));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(2, 3, 3)));
    grid.mark_occupied_coord(VectorI<3>(2, 3, 3));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(3, 1, 1)));
    grid.mark_occupied_coord(VectorI<3>(3, 1, 1));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(3, 1, 2)));
    grid.mark_occupied_coord(VectorI<3>(3, 1, 2));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(3, 1, 3)));
    grid.mark_occupied_coord(VectorI<3>(3, 1, 3));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(3, 2, 1)));
    grid.mark_occupied_coord(VectorI<3>(3, 2, 1));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(3, 2, 2)));
    grid.mark_occupied_coord(VectorI<3>(3, 2, 2));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(3, 2, 3)));
    grid.mark_occupied_coord(VectorI<3>(3, 2, 3));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(3, 3, 1)));
    grid.mark_occupied_coord(VectorI<3>(3, 3, 1));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(3, 3, 2)));
    grid.mark_occupied_coord(VectorI<3>(3, 3, 2));
    EXPECT_FALSE(grid.is_occupied_coord(VectorI<3>(3, 3, 3)));
    grid.mark_occupied_coord(VectorI<3>(3, 3, 3));


    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 1, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 1, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 1, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 2, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 2, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 2, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 3, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 3, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 3, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 1, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 1, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 1, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 2, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 2, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 2, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 3, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 3, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 3, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 1, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 1, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 1, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 2, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 2, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 2, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 3, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 3, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 3, 3)));
    EXPECT_EQ(grid.num_occupied_voxel(), 27);

    auto result = query_knn(grid, grid.aabb.center(), 27);
    EXPECT_EQ(result.indices.size(), 27);
    result = query_knn(grid, grid.aabb.center(), 12);
    EXPECT_EQ(result.indices.size(), 12);
    VectorS<3> query = grid.aabb.center() - grid.voxel_side_length();
    result = query_knn(grid, query, 27);
    EXPECT_EQ(result.indices.size(), 27);
}

TEST(TestOccupancyGrid, query_kring) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());
    grid.mark_occupied_coord(VectorI<3>(1, 1, 1));
    grid.mark_occupied_coord(VectorI<3>(1, 1, 2));
    grid.mark_occupied_coord(VectorI<3>(1, 1, 3));
    grid.mark_occupied_coord(VectorI<3>(1, 2, 1));
    grid.mark_occupied_coord(VectorI<3>(1, 2, 2));
    grid.mark_occupied_coord(VectorI<3>(1, 2, 3));
    grid.mark_occupied_coord(VectorI<3>(1, 3, 1));
    grid.mark_occupied_coord(VectorI<3>(1, 3, 2));
    grid.mark_occupied_coord(VectorI<3>(1, 3, 3));
    grid.mark_occupied_coord(VectorI<3>(2, 1, 1));
    grid.mark_occupied_coord(VectorI<3>(2, 1, 2));
    grid.mark_occupied_coord(VectorI<3>(2, 1, 3));
    grid.mark_occupied_coord(VectorI<3>(2, 2, 1));
    grid.mark_occupied_coord(VectorI<3>(2, 2, 2));
    grid.mark_occupied_coord(VectorI<3>(2, 2, 3));
    grid.mark_occupied_coord(VectorI<3>(2, 3, 1));
    grid.mark_occupied_coord(VectorI<3>(2, 3, 2));
    grid.mark_occupied_coord(VectorI<3>(2, 3, 3));
    grid.mark_occupied_coord(VectorI<3>(3, 1, 1));
    grid.mark_occupied_coord(VectorI<3>(3, 1, 2));
    grid.mark_occupied_coord(VectorI<3>(3, 1, 3));
    grid.mark_occupied_coord(VectorI<3>(3, 2, 1));
    grid.mark_occupied_coord(VectorI<3>(3, 2, 2));
    grid.mark_occupied_coord(VectorI<3>(3, 2, 3));
    grid.mark_occupied_coord(VectorI<3>(3, 3, 1));
    grid.mark_occupied_coord(VectorI<3>(3, 3, 2));
    grid.mark_occupied_coord(VectorI<3>(3, 3, 3));

    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 1, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 1, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 1, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 2, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 2, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 2, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 3, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 3, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 3, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 1, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 1, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 1, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 2, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 2, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 2, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 3, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 3, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 3, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 1, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 1, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 1, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 2, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 2, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 2, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 3, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 3, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 3, 3)));
    EXPECT_EQ(grid.num_occupied_voxel(), 27);

    auto result = query_kring(grid, grid.aabb.center(), 1);
    EXPECT_EQ(result.indices.size(), 27);
    VectorS<3> query = grid.aabb.center() - grid.voxel_side_length();
    EXPECT_EQ(grid.to_coord(query), VectorI<3>(1, 1, 1));
    result = query_kring(grid, query, 1);
    EXPECT_EQ(result.indices.size(), 8);
}


TEST(TestOccupancyGrid, query_radius) {
    occupancy_grid grid({5, 5, 5}, aligned_box3::unit());
    grid.mark_occupied_coord(VectorI<3>(1, 1, 1));
    grid.mark_occupied_coord(VectorI<3>(1, 1, 2));
    grid.mark_occupied_coord(VectorI<3>(1, 1, 3));
    grid.mark_occupied_coord(VectorI<3>(1, 2, 1));
    grid.mark_occupied_coord(VectorI<3>(1, 2, 2));
    grid.mark_occupied_coord(VectorI<3>(1, 2, 3));
    grid.mark_occupied_coord(VectorI<3>(1, 3, 1));
    grid.mark_occupied_coord(VectorI<3>(1, 3, 2));
    grid.mark_occupied_coord(VectorI<3>(1, 3, 3));
    grid.mark_occupied_coord(VectorI<3>(2, 1, 1));
    grid.mark_occupied_coord(VectorI<3>(2, 1, 2));
    grid.mark_occupied_coord(VectorI<3>(2, 1, 3));
    grid.mark_occupied_coord(VectorI<3>(2, 2, 1));
    grid.mark_occupied_coord(VectorI<3>(2, 2, 2));
    grid.mark_occupied_coord(VectorI<3>(2, 2, 3));
    grid.mark_occupied_coord(VectorI<3>(2, 3, 1));
    grid.mark_occupied_coord(VectorI<3>(2, 3, 2));
    grid.mark_occupied_coord(VectorI<3>(2, 3, 3));
    grid.mark_occupied_coord(VectorI<3>(3, 1, 1));
    grid.mark_occupied_coord(VectorI<3>(3, 1, 2));
    grid.mark_occupied_coord(VectorI<3>(3, 1, 3));
    grid.mark_occupied_coord(VectorI<3>(3, 2, 1));
    grid.mark_occupied_coord(VectorI<3>(3, 2, 2));
    grid.mark_occupied_coord(VectorI<3>(3, 2, 3));
    grid.mark_occupied_coord(VectorI<3>(3, 3, 1));
    grid.mark_occupied_coord(VectorI<3>(3, 3, 2));
    grid.mark_occupied_coord(VectorI<3>(3, 3, 3));

    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 1, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 1, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 1, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 2, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 2, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 2, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 3, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 3, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(1, 3, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 1, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 1, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 1, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 2, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 2, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 2, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 3, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 3, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(2, 3, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 1, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 1, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 1, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 2, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 2, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 2, 3)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 3, 1)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 3, 2)));
    EXPECT_TRUE(grid.is_occupied_coord(VectorI<3>(3, 3, 3)));
    EXPECT_EQ(grid.num_occupied_voxel(), 27);

    auto result = query_radius(grid, grid.aabb.center(), grid.voxel_side_length().norm());
    EXPECT_LE(result.indices.size(), grid.num_occupied_voxel());
    VectorS<3> query = grid.aabb.center() - grid.voxel_side_length();
    EXPECT_EQ(grid.to_coord(query), VectorI<3>(1, 1, 1));
    result = query_radius(grid, query, grid.voxel_side_length().norm());
    EXPECT_LE(result.indices.size(), grid.num_occupied_voxel());
}