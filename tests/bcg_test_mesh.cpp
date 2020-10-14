//
// Created by alex on 14.10.20.
//

#include <gtest/gtest.h>

#include "bcg_library/geometry/bcg_mesh.h"

using namespace bcg;

class TestMeshFixture : public ::testing::Test{
public:
    TestMeshFixture() = default;

    halfedge_mesh mesh;
};

TEST_F(TestMeshFixture, constructors){
    EXPECT_TRUE(mesh.positions);
    EXPECT_TRUE(mesh.vconn);
    EXPECT_TRUE(mesh.hconn);
    EXPECT_TRUE(mesh.fconn);
}