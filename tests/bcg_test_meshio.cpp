//
// Created by alex on 14.10.20.
//

#include <gtest/gtest.h>

#include "bcg_library/geometry/bcg_meshio.h"
#include "bcg_library/geometry/bcg_mesh_factory.h"

#ifdef _WIN32
static std::string test_data_path = "..\\..\\tests\\data\\";
#else
static std::string test_data_path = "../../tests/data/";
#endif


using namespace bcg;

class TestMeshIoFixture : public ::testing::Test{
public:
    TestMeshIoFixture() = default;

    halfedge_mesh mesh;
};

TEST_F(TestMeshIoFixture, read_write_off){
    meshio read_io(test_data_path + "test_mesh.off", meshio_flags());
    read_io.read(mesh);

    EXPECT_EQ(mesh.num_vertices(), 3835);
    EXPECT_EQ(mesh.num_faces(), 7646);

    mesh_factory factory;
    halfedge_mesh mesh_write_triangle = factory.make_triangle();
    meshio write_io(test_data_path + "test_triangle.off", meshio_flags());
    write_io.write(mesh_write_triangle);
    halfedge_mesh mesh_read_triangle;
    write_io.read(mesh_read_triangle);
    EXPECT_EQ(mesh_write_triangle, mesh_read_triangle);
}

TEST_F(TestMeshIoFixture, obj){
    meshio io(test_data_path + "test_mesh.obj", meshio_flags());
    io.read(mesh);

    EXPECT_EQ(mesh.num_vertices(), 33);
    EXPECT_EQ(mesh.num_faces(), 32);
}

TEST_F(TestMeshIoFixture, stl){
    meshio io(test_data_path + "test_mesh.stl", meshio_flags());
    io.read(mesh);

    EXPECT_EQ(mesh.num_vertices(), 44020);
    EXPECT_EQ(mesh.num_faces(), 88040);
}

TEST_F(TestMeshIoFixture, ply){
    meshio io(test_data_path + "test_mesh.ply", meshio_flags());
    io.read(mesh);

    EXPECT_EQ(mesh.num_vertices(), 16243);
    EXPECT_EQ(mesh.num_faces(), 31532);
}