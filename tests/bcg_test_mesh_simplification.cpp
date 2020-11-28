//
// Created by alex on 27.11.20.
//
#include <gtest/gtest.h>

#include "geometry/mesh/bcg_mesh.h"
#include "geometry/mesh/bcg_meshio.h"
#include "geometry/mesh/bcg_mesh_simplification.h"
#include "geometry/mesh/bcg_mesh_features.h"

#ifdef _WIN32
static std::string test_data_path = "..\\tests\\";
#else
static std::string test_data_path = "../tests/";
#endif

using namespace bcg;


class SurfaceSimplificationTest : public ::testing::Test {
public:
    SurfaceSimplificationTest() {

    }

    halfedge_mesh mesh;
};

// plain simplification test
TEST_F(SurfaceSimplificationTest, simplification) {
    meshio read_io(test_data_path + "pmp-data/off/bunny_adaptive.off", meshio_flags());
    EXPECT_TRUE(read_io.read(mesh));
    mesh_simplification(mesh, mesh.vertices.size() * 0.1,
                        5,
                        0.01,   // edge length
                        10,     // max valence
                        10,     // normal deviation
                        0.001); // Hausdorff
    EXPECT_EQ(mesh.vertices.size(), size_t(3800));
    EXPECT_EQ(mesh.faces.size(), size_t(7596));
}

// simplify with feature edge preservation enabled
TEST_F(SurfaceSimplificationTest, simplification_with_features) {
    meshio read_io(test_data_path + "pmp-data/off/icosahedron_subdiv.off", meshio_flags());
    EXPECT_TRUE(read_io.read(mesh));
    mesh_features(mesh, false, true, 25);

    mesh_simplification(mesh, mesh.vertices.size() * 0.1,
                        5,
                        0,   // edge length
                        0,     // max valence
                        0,     // normal deviation
                        0); // Hausdorff
    EXPECT_EQ(mesh.vertices.size(), size_t(64));
}
