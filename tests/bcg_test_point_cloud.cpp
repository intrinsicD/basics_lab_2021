//
// Created by alex on 14.10.20.
//

#include <gtest/gtest.h>

#include "bcg_library/geometry/bcg_point_cloud.h"

using namespace bcg;

class TestPointCloudFixture : public ::testing::Test{
public:
    TestPointCloudFixture(){
        pc_full.add_vertex({0, 0, 1});
        pc_full.add_vertex({0, 1, 0});
        pc_full.add_vertex({0, 1, 1});
        pc_full.add_vertex({1, 0, 1});
        pc_full.add_vertex({0, 0, 1+1e-6f});
    }

    point_cloud pc_empty;
    point_cloud pc_full;
};

TEST_F(TestPointCloudFixture, empty) {
    EXPECT_TRUE(pc_empty.empty());
    EXPECT_FALSE(pc_full.empty());
    EXPECT_TRUE(pc_empty.positions);
    EXPECT_TRUE(pc_empty.vertices_deleted);
    EXPECT_EQ(pc_empty.vertices.num_properties(), 2);
    EXPECT_EQ(pc_full.vertices_deleted.size(), 5);
}

TEST_F(TestPointCloudFixture, has_garbage) {
    EXPECT_FALSE(pc_empty.has_garbage());
    EXPECT_FALSE(pc_full.has_garbage());
    EXPECT_EQ(pc_full.num_vertices(), pc_full.vertices.size());
    pc_full.delete_vertex(4);
    EXPECT_FALSE(pc_full.vertices_deleted[3]);
    EXPECT_TRUE(pc_full.vertices_deleted[4]);
    EXPECT_EQ(pc_full.num_vertices(), pc_full.vertices.size() - 1);
    EXPECT_TRUE(pc_full.has_garbage());
}

TEST_F(TestPointCloudFixture, garbage_collection) {
    EXPECT_FALSE(pc_empty.has_garbage());
    EXPECT_FALSE(pc_full.has_garbage());
    pc_full.delete_vertex(4);
    EXPECT_TRUE(pc_full.has_garbage());

    pc_full.garbage_collection();
    EXPECT_FALSE(pc_full.has_garbage());
    EXPECT_FALSE(pc_full.vertices.is_valid(4));
}


TEST_F(TestPointCloudFixture, Vertices) {
    pc_empty.vertices.resize(10);

    EXPECT_TRUE(pc_empty.positions);
    EXPECT_TRUE(pc_empty.vertices_deleted);

    size_t count = 0;
    for (const auto v : pc_empty.vertices) {
        EXPECT_TRUE(pc_empty.vertices.is_valid(v));
        EXPECT_FALSE(pc_empty.vertices_deleted[v]);
        ++count;
    }
    EXPECT_EQ(pc_empty.vertices.size(), count);
    EXPECT_EQ(pc_empty.num_vertices(), count);
}

TEST_F(TestPointCloudFixture, add_vertex){
    EXPECT_TRUE(pc_empty.empty());
    EXPECT_EQ(pc_empty.num_vertices(), 0);
    EXPECT_FALSE(pc_empty.vertices.is_dirty());
    auto v0 = pc_empty.add_vertex({0, 0, 1});
    auto v1 = pc_empty.add_vertex({0, 1, 0});
    auto v2 = pc_empty.add_vertex({0, 1, 1});
    auto v3 = pc_empty.add_vertex({1, 0, 1});
    auto v4 = pc_empty.add_vertex({0, 0, 1+1e-6f});
    EXPECT_TRUE(pc_empty.vertices.is_valid(v0));
    EXPECT_TRUE(pc_empty.vertices.is_valid(v1));
    EXPECT_TRUE(pc_empty.vertices.is_valid(v2));
    EXPECT_TRUE(pc_empty.vertices.is_valid(v3));
}

TEST_F(TestPointCloudFixture, assign){
    EXPECT_FALSE(pc_full.has_garbage());
    pc_full.delete_vertex(4);
    pc_empty.assign(pc_full);
    EXPECT_NE(pc_empty.positions.shared_ptr(), pc_full.positions.shared_ptr());
    for(size_t i = 0; i < pc_empty.vertices.size(); ++i){
        EXPECT_EQ(pc_empty.positions[i], pc_full.positions[i]);
    }
    for(const auto v : pc_empty.vertices){
        EXPECT_FALSE(pc_empty.vertices_deleted[v]);
    }
    EXPECT_TRUE(pc_empty.has_garbage());
    EXPECT_TRUE(pc_empty.vertices_deleted[4]);
    EXPECT_EQ(pc_empty.size_vertices_deleted, pc_full.size_vertices_deleted);
}

TEST_F(TestPointCloudFixture, operator_assign){
    EXPECT_FALSE(pc_full.has_garbage());
    pc_full.delete_vertex(4);
    pc_empty = pc_full;
    EXPECT_EQ(pc_empty.positions.shared_ptr(), pc_full.positions.shared_ptr());
    for(size_t i = 0; i < pc_empty.vertices.size(); ++i){
        EXPECT_EQ(pc_empty.positions[i], pc_full.positions[i]);
    }
    for(const auto v : pc_empty.vertices){
        EXPECT_FALSE(pc_empty.vertices_deleted[v]);
    }
    EXPECT_TRUE(pc_empty.has_garbage());
    EXPECT_TRUE(pc_empty.vertices_deleted[4]);
    EXPECT_EQ(pc_empty.size_vertices_deleted, pc_full.size_vertices_deleted);
}

TEST_F(TestPointCloudFixture, find_closest) {
    EXPECT_EQ(pc_full.find_closest_vertex(VectorS<3>(0, 0, 1-1e-6f)), vertex_handle(0));
    auto result = pc_full.find_closest_k_vertices(pc_full.positions[0], 2);
    EXPECT_EQ(result[0], vertex_handle(0));
    EXPECT_EQ(result[1], vertex_handle(4));
    result = pc_full.find_closest_vertices_radius(VectorS<3>(0, 0, 1-1e-6f), 0.01);
    EXPECT_EQ(result[0], vertex_handle(0));
    EXPECT_EQ(result[1], vertex_handle(4));
    EXPECT_EQ(result.size(), 2);

    EXPECT_FALSE(pc_full.vertices.is_valid(vertex_handle(5)));

    EXPECT_TRUE(pc_full.vertices.is_dirty());
    EXPECT_FALSE(pc_full.empty());
    EXPECT_EQ(pc_full.num_vertices(), 5);
    pc_full.positions.set_clean();
    pc_full.vertices_deleted.set_clean();
    EXPECT_FALSE(pc_full.vertices.is_dirty());
    pc_full.delete_vertex(0);
    EXPECT_TRUE(pc_full.vertices_deleted[0]);
    EXPECT_FALSE(pc_full.vertices_deleted[1]);

    EXPECT_TRUE(pc_full.vertices.is_dirty());
    EXPECT_TRUE(pc_full.has_garbage());
    EXPECT_EQ(pc_full.num_vertices(), 4);
    EXPECT_EQ(pc_full.vertices.size(), 5);
    EXPECT_EQ(pc_full.size_vertices_deleted, 1);

    pc_full.garbage_collection();

    EXPECT_FALSE(pc_full.has_garbage());
    EXPECT_EQ(pc_full.num_vertices(), 4);
    EXPECT_EQ(pc_full.vertices.size(), 4);
    EXPECT_EQ(pc_full.size_vertices_deleted, 0);
}