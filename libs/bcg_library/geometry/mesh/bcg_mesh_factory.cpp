//
// Created by alex on 14.10.20.
//

#include "bcg_mesh_factory.h"
#include "bcg_mesh_dual.h"

namespace bcg {

halfedge_mesh mesh_factory::make_triangle() {
    return make_triangle(triangle3(zero3s, {0, 1, 0}, {1, 0, 0}));
}

halfedge_mesh mesh_factory::make_triangle(const triangle3 &triangle) {
    halfedge_mesh mesh;

    for (const auto &point : triangle.points) {
        mesh.add_vertex(point);
    }

    auto fnormals = mesh.faces.add<VectorS<3>, 3>("f_normal");
    auto f = mesh.add_triangle(0, 1, 2);
    fnormals[f] = normal(triangle);
    return mesh;
}

halfedge_mesh mesh_factory::make_quad() {
    return make_quad(quad3(
            VectorS<3>(-0.5, 0.5, 0),
            VectorS<3>(-0.5, -0.5, 0),
            VectorS<3>(0.5, -0.5, 0),
            VectorS<3>(0.5, 0.5, 0)));
}

halfedge_mesh mesh_factory::make_quad(const quad3 &quad) {
    halfedge_mesh mesh;

    for (const auto &point : quad.points) {
        mesh.add_vertex(point);
    }

    auto f = mesh.add_quad(0, 1, 2, 3);
    mesh.triangulate(f);
    return mesh;
}

halfedge_mesh mesh_factory::make_parameterized_plane() {
    return make_parameterized_plane(ParameterizedPlane3());
}

halfedge_mesh mesh_factory::make_parameterized_plane(const ParameterizedPlane3 &plane) {
    halfedge_mesh mesh;

    auto vertices = get_vertices(plane);

    for (const auto &point : vertices) {
        mesh.add_vertex(point);
    }

    auto faces = get_faces(plane);
    VectorI<3> face0 = faces[0];
    VectorI<3> face1 = faces[1];
    mesh.add_face({face0[0], face0[1], face0[2]});
    mesh.add_face({face1[0], face1[1], face1[2]});

    return mesh;
}

halfedge_mesh mesh_factory::make_icosahedron() {
    halfedge_mesh mesh;
    const auto X = 0.525731112119133606;
    const auto Z = 0.850650808352039932;
    const auto N = 0.0;

    auto v = mesh.add_vertex(VectorS<3>(-X, N, Z));
    v = mesh.add_vertex(VectorS<3>(X, N, Z));
    v = mesh.add_vertex(VectorS<3>(-X, N, -Z));
    v = mesh.add_vertex(VectorS<3>(X, N, -Z));
    v = mesh.add_vertex(VectorS<3>(N, Z, X));
    v = mesh.add_vertex(VectorS<3>(N, Z, -X));
    v = mesh.add_vertex(VectorS<3>(N, -Z, X));
    v = mesh.add_vertex(VectorS<3>(N, -Z, -X));
    v = mesh.add_vertex(VectorS<3>(Z, X, N));
    v = mesh.add_vertex(VectorS<3>(-Z, X, N));
    v = mesh.add_vertex(VectorS<3>(Z, -X, N));
    v = mesh.add_vertex(VectorS<3>(-Z, -X, N));

    auto f = mesh.add_triangle(4, 0, 1);
    f = mesh.add_triangle(9, 0, 4);
    f = mesh.add_triangle(5, 9, 4);
    f = mesh.add_triangle(5, 4, 8);
    f = mesh.add_triangle(8, 4, 1);
    f = mesh.add_triangle(10, 8, 1);
    f = mesh.add_triangle(3, 8, 10);
    f = mesh.add_triangle(3, 5, 8);
    f = mesh.add_triangle(2, 5, 3);
    f = mesh.add_triangle(7, 2, 3);
    f = mesh.add_triangle(10, 7, 3);
    f = mesh.add_triangle(6, 7, 10);
    f = mesh.add_triangle(11, 7, 6);
    f = mesh.add_triangle(0, 11, 6);
    f = mesh.add_triangle(1, 0, 6);
    f = mesh.add_triangle(1, 6, 10);
    f = mesh.add_triangle(0, 9, 11);
    f = mesh.add_triangle(11, 9, 2);
    f = mesh.add_triangle(2, 9, 5);
    f = mesh.add_triangle(2, 7, 11);
    return mesh;
}

halfedge_mesh mesh_factory::make_tetrahedron() {
    halfedge_mesh mesh;

    // choose coordinates on the unit sphere
    bcg_scalar_t a = 1.0 / 3.0;
    bcg_scalar_t b = sqrt(8.0 / 9.0);
    bcg_scalar_t c = sqrt(2.0 / 9.0);
    bcg_scalar_t d = sqrt(2.0 / 3.0);

    // add the 4 vertices
    auto v0 = mesh.add_vertex(VectorS<3>(0, 0, 1));
    auto v1 = mesh.add_vertex(VectorS<3>(-c, d, -a));
    auto v2 = mesh.add_vertex(VectorS<3>(-c, -d, -a));
    auto v3 = mesh.add_vertex(VectorS<3>(b, 0, -a));

    // add the 4 faces
    mesh.add_triangle(v0, v1, v2);
    mesh.add_triangle(v0, v2, v3);
    mesh.add_triangle(v0, v3, v1);
    mesh.add_triangle(v3, v2, v1);

    return mesh;
}

halfedge_mesh mesh_factory::make_hexahedron() {
    halfedge_mesh mesh;

    // choose coordinates on the unit sphere
    bcg_scalar_t a = 1.0 / sqrt(3.0);

    // add the 8 vertices
    auto v0 = mesh.add_vertex(VectorS<3>(-a, -a, -a));
    auto v1 = mesh.add_vertex(VectorS<3>(a, -a, -a));
    auto v2 = mesh.add_vertex(VectorS<3>(a, a, -a));
    auto v3 = mesh.add_vertex(VectorS<3>(-a, a, -a));
    auto v4 = mesh.add_vertex(VectorS<3>(-a, -a, a));
    auto v5 = mesh.add_vertex(VectorS<3>(a, -a, a));
    auto v6 = mesh.add_vertex(VectorS<3>(a, a, a));
    auto v7 = mesh.add_vertex(VectorS<3>(-a, a, a));

    // add the 6 faces
    mesh.add_quad(v3, v2, v1, v0);
    mesh.add_quad(v2, v6, v5, v1);
    mesh.add_quad(v5, v6, v7, v4);
    mesh.add_quad(v0, v4, v7, v3);
    mesh.add_quad(v3, v7, v6, v2);
    mesh.add_quad(v1, v5, v4, v0);

    return mesh;
}

void project_to_unit_sphere(halfedge_mesh &mesh) {
    for (auto v : mesh.vertices) {
        auto p = mesh.positions[v];
        auto n = p.norm();
        mesh.positions[v] = (1.0 / n) * p;
    }
}

halfedge_mesh mesh_factory::make_octahedron() {
    auto mesh = make_hexahedron();
    mesh_dual(mesh);
    project_to_unit_sphere(mesh);
    return mesh;
}

halfedge_mesh mesh_factory::make_dodecahedron(){
    auto mesh = make_icosahedron();
    mesh_dual(mesh);
    project_to_unit_sphere(mesh);
    return mesh;
}

halfedge_mesh mesh_factory::make_box() {
    return make_box(aligned_box3(zero3s, one3s));
}

halfedge_mesh mesh_factory::make_box(const aligned_box3 &aabb) {
    halfedge_mesh mesh;
    auto v0 = mesh.add_vertex(aabb.min);
    auto v1 = mesh.add_vertex({aabb.max[0], aabb.min[1], aabb.min[2]});
    auto v2 = mesh.add_vertex({aabb.min[0], aabb.min[1], aabb.max[2]});
    auto v3 = mesh.add_vertex({aabb.max[0], aabb.min[1], aabb.max[2]});
    auto v4 = mesh.add_vertex({aabb.min[0], aabb.max[1], aabb.min[2]});
    auto v5 = mesh.add_vertex({aabb.max[0], aabb.max[1], aabb.min[2]});
    auto v6 = mesh.add_vertex({aabb.min[0], aabb.max[1], aabb.max[2]});
    auto v7 = mesh.add_vertex(aabb.max);

    mesh.add_quad(v0, v1, v3, v2);
    mesh.add_quad(v1, v5, v7, v3);
    mesh.add_quad(v2, v3, v7, v6);
    mesh.add_quad(v4, v5, v1, v0);
    mesh.add_quad(v4, v0, v2, v6);
    mesh.add_quad(v6, v7, v5, v4);

    mesh.triangulate();

    return mesh;
}

}