//
// Created by alex on 14.10.20.
//

#include <gtest/gtest.h>

#include "geometry/mesh/bcg_mesh.h"
#include "geometry/mesh/bcg_meshio.h"
#include "geometry/mesh/bcg_mesh_factory.h"

using namespace bcg;

#ifdef _WIN32
static std::string test_data_path = "..\\tests\\data\\";
#else
static std::string test_data_path = "../tests/";
#endif

class HalfedgeMeshTest : public ::testing::Test {
public:
    halfedge_mesh mesh;
    vertex_handle v0, v1, v2, v3;
    halfedge_handle h0;
    face_handle f0;

    bool read(std::string filename) {
        meshio reader(filename, meshio_flags());
        return reader.read(mesh);
    }

    void add_triangle() {
        v0 = mesh.add_vertex(VectorS<3>(0, 0, 0));
        v1 = mesh.add_vertex(VectorS<3>(1, 0, 0));
        v2 = mesh.add_vertex(VectorS<3>(0, 1, 0));
        f0 = mesh.add_triangle(v0, v1, v2);
    }

    void add_quad() {
        v0 = mesh.add_vertex(VectorS<3>(0, 0, 0));
        v1 = mesh.add_vertex(VectorS<3>(1, 0, 0));
        v2 = mesh.add_vertex(VectorS<3>(1, 1, 0));
        v3 = mesh.add_vertex(VectorS<3>(0, 1, 0));
        f0 = mesh.add_quad(v0, v1, v2, v3);
    }
};

TEST_F(HalfedgeMeshTest, emptyMesh) {
    EXPECT_EQ(mesh.num_vertices(), size_t(0));
    EXPECT_EQ(mesh.num_edges(), size_t(0));
    EXPECT_EQ(mesh.num_faces(), size_t(0));
}

TEST_F(HalfedgeMeshTest, insert_remove_single_vertex) {
    auto v0 = mesh.add_vertex(VectorS<3>(0, 0, 0));
    EXPECT_EQ(mesh.num_vertices(), size_t(1));
    mesh.delete_vertex(v0);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.num_vertices(), size_t(0));
}

TEST_F(HalfedgeMeshTest, insert_remove_single_triangle) {
    auto v0 = mesh.add_vertex(VectorS<3>(0, 0, 0));
    auto v1 = mesh.add_vertex(VectorS<3>(1, 0, 0));
    auto v2 = mesh.add_vertex(VectorS<3>(0, 1, 0));
    auto f0 = mesh.add_triangle(v0, v1, v2);
    EXPECT_EQ(mesh.num_vertices(), size_t(3));
    EXPECT_EQ(mesh.num_edges(), size_t(3));
    EXPECT_EQ(mesh.num_faces(), size_t(1));
    mesh.delete_face(f0);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.num_vertices(), size_t(0));
    EXPECT_EQ(mesh.num_edges(), size_t(0));
    EXPECT_EQ(mesh.num_faces(), size_t(0));
}

TEST_F(HalfedgeMeshTest, insert_remove_single_quad) {
    auto v0 = mesh.add_vertex(VectorS<3>(0, 0, 0));
    auto v1 = mesh.add_vertex(VectorS<3>(1, 0, 0));
    auto v2 = mesh.add_vertex(VectorS<3>(1, 1, 0));
    auto v3 = mesh.add_vertex(VectorS<3>(0, 1, 0));
    auto f0 = mesh.add_quad(v0, v1, v2, v3);
    EXPECT_EQ(mesh.num_vertices(), size_t(4));
    EXPECT_EQ(mesh.num_edges(), size_t(4));
    EXPECT_EQ(mesh.num_faces(), size_t(1));
    mesh.delete_face(f0);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.num_vertices(), size_t(0));
    EXPECT_EQ(mesh.num_edges(), size_t(0));
    EXPECT_EQ(mesh.num_faces(), size_t(0));
}

TEST_F(HalfedgeMeshTest, insert_remove_single_polygonal_face) {
    std::vector<vertex_handle> vertices(4);
    vertices[0] = mesh.add_vertex(VectorS<3>(0, 0, 0));
    vertices[1] = mesh.add_vertex(VectorS<3>(1, 0, 0));
    vertices[2] = mesh.add_vertex(VectorS<3>(1, 1, 0));
    vertices[3] = mesh.add_vertex(VectorS<3>(0, 1, 0));

    auto f0 = mesh.add_face(vertices);
    EXPECT_EQ(mesh.num_vertices(), size_t(4));
    EXPECT_EQ(mesh.num_edges(), size_t(4));
    EXPECT_EQ(mesh.num_faces(), size_t(1));
    mesh.delete_face(f0);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.num_vertices(), size_t(0));
    EXPECT_EQ(mesh.num_edges(), size_t(0));
    EXPECT_EQ(mesh.num_faces(), size_t(0));
}

TEST_F(HalfedgeMeshTest, delete_center_vertex) {
    ASSERT_TRUE(read(test_data_path + "pmp-data/off/vertex_onering.off"));
    EXPECT_EQ(mesh.num_vertices(), size_t(7));
    EXPECT_EQ(mesh.num_faces(), size_t(6));
    vertex_handle v0(3); // the central vertex
    mesh.delete_vertex(v0);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.num_vertices(), size_t(0));
    EXPECT_EQ(mesh.num_faces(), size_t(0));
}

TEST_F(HalfedgeMeshTest, delete_center_edge) {
    ASSERT_TRUE(read(test_data_path + "pmp-data/off/edge_onering.off"));
    EXPECT_EQ(mesh.num_vertices(), size_t(10));
    EXPECT_EQ(mesh.num_faces(), size_t(10));
    // the two vertices of the center edge
    vertex_handle v0(4);
    vertex_handle v1(5);

    auto e = mesh.find_edge(v0, v1);
    mesh.delete_edge(e);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.num_vertices(), size_t(10));
    EXPECT_EQ(mesh.num_faces(), size_t(8));
}

TEST_F(HalfedgeMeshTest, copy) {
    auto v0 = mesh.add_vertex(VectorS<3>(0, 0, 0));
    auto v1 = mesh.add_vertex(VectorS<3>(1, 0, 0));
    auto v2 = mesh.add_vertex(VectorS<3>(0, 1, 0));
    f0 = mesh.add_triangle(v0, v1, v2);

    halfedge_mesh m2 = mesh;
    EXPECT_EQ(m2.num_vertices(), size_t(3));
    EXPECT_EQ(m2.num_edges(), size_t(3));
    EXPECT_EQ(m2.num_faces(), size_t(1));
}

TEST_F(HalfedgeMeshTest, assignment) {
    auto v0 = mesh.add_vertex(VectorS<3>(0, 0, 0));
    auto v1 = mesh.add_vertex(VectorS<3>(1, 0, 0));
    auto v2 = mesh.add_vertex(VectorS<3>(0, 1, 0));
    f0 = mesh.add_triangle(v0, v1, v2);

    halfedge_mesh m2;
    m2.assign(mesh);
    EXPECT_EQ(m2.num_vertices(), size_t(3));
    EXPECT_EQ(m2.num_edges(), size_t(3));
    EXPECT_EQ(m2.num_faces(), size_t(1));
}


TEST_F(HalfedgeMeshTest, vertex_properties) {
    add_triangle();

    auto osize = mesh.vertices.properties().size();

    // explicit add
    auto vidx = mesh.vertices.add<int, 1>("v:idx");
    vidx[v0] = 0;
    EXPECT_EQ(mesh.vertices.properties().size(), osize + 1);
    mesh.vertices.remove(vidx);
    EXPECT_EQ(mesh.vertices.properties().size(), osize);

    // implicit add
    vidx = mesh.vertices.get_or_add<int, 1>("v:idx2");
    EXPECT_EQ(mesh.vertices.properties().size(), osize + 1);
    mesh.vertices.remove(vidx);
    EXPECT_EQ(mesh.vertices.properties().size(), osize);
}

TEST_F(HalfedgeMeshTest, halfedge_properties) {
    add_triangle();
    // explicit add
    auto hidx = mesh.halfedges.add<int, 1>("h:idx");
    auto h = mesh.halfedge_graph::get_halfedge(v0);
    hidx[h] = 0;
    EXPECT_EQ(mesh.halfedges.num_properties(), size_t(3));
    mesh.halfedges.remove(hidx);
    EXPECT_EQ(mesh.halfedges.num_properties(), size_t(2));

    // implicit add
    hidx = mesh.halfedges.get_or_add<int, 1>("h:idx2");
    EXPECT_EQ(mesh.halfedges.num_properties(), size_t(3));
    mesh.halfedges.remove(hidx);
    EXPECT_EQ(mesh.halfedges.num_properties(), size_t(2));
}

TEST_F(HalfedgeMeshTest, edge_properties) {
    add_triangle();
    // explicit add
    auto eidx = mesh.edges.add<int, 1>("e:idx");
    auto e = mesh.get_edge(mesh.halfedge_graph::get_halfedge(v0));
    eidx[e] = 0;
    EXPECT_EQ(mesh.edges.num_properties(), size_t(2));
    mesh.edges.remove(eidx);
    EXPECT_EQ(mesh.edges.num_properties(), size_t(1));

    // implicit add
    eidx = mesh.edges.get_or_add<int, 1>("e:idx2");
    EXPECT_EQ(mesh.edges.num_properties(), size_t(2));
    mesh.edges.remove(eidx);
    EXPECT_EQ(mesh.edges.num_properties(), size_t(1));
}

TEST_F(HalfedgeMeshTest, face_properties) {
    add_triangle();
    // explicit add
    auto fidx = mesh.faces.add<int, 1>("f:idx");
    fidx[f0] = 0;
    EXPECT_EQ(mesh.faces.num_properties(), size_t(3));
    mesh.faces.remove(fidx);
    EXPECT_EQ(mesh.faces.num_properties(), size_t(2));

    // implicit add
    fidx = mesh.faces.get_or_add<int, 1>("f:idx2");
    EXPECT_EQ(mesh.faces.num_properties(), size_t(3));
    mesh.faces.remove(fidx);
    EXPECT_EQ(mesh.faces.num_properties(), size_t(2));
}

TEST_F(HalfedgeMeshTest, vertex_iterators) {
    add_triangle();
    size_t sumIdx(0);
    for (const auto v : mesh.vertices) {
        sumIdx += v.idx;
    }
    EXPECT_EQ(sumIdx, size_t(3));
}

TEST_F(HalfedgeMeshTest, edge_iterators) {
    add_triangle();
    size_t sumIdx(0);
    for (const auto e : mesh.edges) {
        sumIdx += e.idx;
    }
    EXPECT_EQ(sumIdx, size_t(3));
}

TEST_F(HalfedgeMeshTest, halfedge_iterators) {
    add_triangle();
    size_t sumIdx(0);
    for (const auto h : mesh.halfedges) {
        sumIdx += h.idx;
    }
    EXPECT_EQ(sumIdx, size_t(15));
}

TEST_F(HalfedgeMeshTest, face_iterators) {
    add_triangle();
    add_triangle();
    add_triangle();
    size_t sumIdx(0);
    for (const auto f : mesh.faces) {
        sumIdx += f.idx;
    }
    EXPECT_EQ(sumIdx, size_t(3));
}

TEST_F(HalfedgeMeshTest, is_triangle_mesh) {
    add_triangle();
    EXPECT_TRUE(mesh.is_triangle_mesh());
    EXPECT_FALSE(mesh.is_quad_mesh());
    add_quad();
    EXPECT_FALSE(mesh.is_triangle_mesh());
    EXPECT_FALSE(mesh.is_quad_mesh());
}

TEST_F(HalfedgeMeshTest, is_quad_mesh) {
    auto v0 = mesh.add_vertex(VectorS<3>(0, 0, 0));
    auto v1 = mesh.add_vertex(VectorS<3>(1, 0, 0));
    auto v2 = mesh.add_vertex(VectorS<3>(1, 1, 0));
    auto v3 = mesh.add_vertex(VectorS<3>(0, 1, 0));
    f0 = mesh.add_quad(v0, v1, v2, v3);
    EXPECT_TRUE(mesh.is_quad_mesh());
    EXPECT_FALSE(mesh.is_triangle_mesh());
    mesh.triangulate();
    EXPECT_TRUE(mesh.is_triangle_mesh());
}

TEST_F(HalfedgeMeshTest, poly_mesh) {
    std::vector<vertex_handle> vertices(5);
    vertices[0] = mesh.add_vertex(VectorS<3>(0, 0, 0));
    vertices[1] = mesh.add_vertex(VectorS<3>(1, 0, 0));
    vertices[2] = mesh.add_vertex(VectorS<3>(1, 1, 0));
    vertices[3] = mesh.add_vertex(VectorS<3>(0.5, 1, 0));
    vertices[4] = mesh.add_vertex(VectorS<3>(0, 1, 0));
    f0 = mesh.add_face(vertices);
    EXPECT_FALSE(mesh.is_triangle_mesh());
    EXPECT_FALSE(mesh.is_quad_mesh());
    mesh.triangulate();
    EXPECT_TRUE(mesh.is_triangle_mesh());
}

TEST_F(HalfedgeMeshTest, valence) {
    add_triangle();
    size_t sumValence(0);
    for (const auto v : mesh.vertices) {
        sumValence += mesh.halfedge_graph::get_valence(v);
    }
    EXPECT_EQ(sumValence, size_t(6));
}

TEST_F(HalfedgeMeshTest, collapse) {
    add_quad();
    mesh.triangulate();
    EXPECT_EQ(mesh.num_faces(), size_t(2));
    h0 = mesh.find_halfedge(v3, v2);
    if (mesh.is_collapse_ok(h0))
        mesh.collapse(h0);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.num_faces(), size_t(1));

    read(test_data_path + "pmp-data/off/vertex_onering.off");
    for (const auto v : mesh.vertices) {
        EXPECT_TRUE(mesh.is_manifold(v));
    }
}

TEST_F(HalfedgeMeshTest, collapse2) {
    read(test_data_path + "pmp-data/off/vertex_onering.off");
    EXPECT_EQ(mesh.num_faces(), size_t(6));
    EXPECT_EQ(mesh.num_edges(), size_t(12));
    EXPECT_EQ(mesh.num_vertices(), size_t(7));
    h0 = mesh.find_halfedge(3, 2);
    if (mesh.is_collapse_ok(h0))
        mesh.collapse(h0);
    mesh.garbage_collection();
    EXPECT_EQ(mesh.num_faces(), size_t(4));
    EXPECT_EQ(mesh.num_edges(), size_t(9));
    EXPECT_EQ(mesh.num_vertices(), size_t(6));
}

TEST_F(HalfedgeMeshTest, face_split) {
    add_quad();
    EXPECT_EQ(mesh.num_faces(), size_t(1));
    VectorS<3> p(0.5, 0.5, 0);
    v0 = mesh.split(f0, p);
    EXPECT_EQ(mesh.num_faces(), size_t(4));
}

TEST_F(HalfedgeMeshTest, edge_split) {
    add_triangle();
    EXPECT_EQ(mesh.num_faces(), size_t(1));
    auto e = mesh.find_edge(v0, v1);
    VectorS<3> p0 = mesh.positions[v0];
    VectorS<3> p1 = mesh.positions[v1];
    VectorS<3> p = (p0 + p1) * 0.5f;
    h0 = mesh.split(e, p);
    EXPECT_EQ(mesh.num_faces(), size_t(2));
}

TEST_F(HalfedgeMeshTest, edge_flip) {
    read(test_data_path + "pmp-data/off/edge_onering.off");
    EXPECT_EQ(mesh.num_vertices(), size_t(10));
    EXPECT_EQ(mesh.num_faces(), size_t(10));

    // the two vertices of the center edge
    vertex_handle v0(4);
    vertex_handle v1(5);
    auto e = mesh.find_edge(v0, v1);
    if (mesh.is_flip_ok(e)) {
        mesh.flip(e);
    }
    EXPECT_EQ(mesh.num_vertices(), size_t(10));
    EXPECT_EQ(mesh.num_faces(), size_t(10));
}

TEST_F(HalfedgeMeshTest, is_manifold) {
    read(test_data_path + "pmp-data/off/vertex_onering.off");
    for (const auto v : mesh.vertices) {
        EXPECT_TRUE(mesh.is_manifold(v));
    }
}

TEST_F(HalfedgeMeshTest, edge_length) {
    add_quad();
    float sum(0);
    for (const auto e : mesh.edges) {
        sum += mesh.get_length(e);
    }
    sum /= (float) mesh.num_edges();
    EXPECT_FLOAT_EQ(sum, float(1));
}