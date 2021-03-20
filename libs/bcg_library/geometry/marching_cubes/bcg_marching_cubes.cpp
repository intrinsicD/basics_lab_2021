//
// Created by alex on 25.11.20.
//

#include "bcg_marching_cubes.h"
#include "bcg_marching_cubes_tables.h"
#include "aligned_box/bcg_aligned_box.h"
#include "math/matrix/bcg_matrix_map_eigen.h"
#include "kdtree/bcg_kdtree.h"
#include "triangle/bcg_triangle.h"

namespace bcg {

std::vector<VectorS<3>> get_vetices_mc(const aligned_box<3> &alignedBox) {
    VectorS<3> center, halfextent;
    alignedBox.get_centered_form(center, halfextent);
    auto min = alignedBox.min;
    auto diag = alignedBox.diagonal();
    std::vector<VectorS<3>> vertices;

    vertices.push_back(min.array() + diag.array() * VectorS<3>(0, 0, 0).array());
    vertices.push_back(min.array() + diag.array() * VectorS<3>(1, 0, 0).array());
    vertices.push_back(min.array() + diag.array() * VectorS<3>(1, 1, 0).array());
    vertices.push_back(min.array() + diag.array() * VectorS<3>(0, 1, 0).array());
    vertices.push_back(min.array() + diag.array() * VectorS<3>(0, 0, 1).array());
    vertices.push_back(min.array() + diag.array() * VectorS<3>(1, 0, 1).array());
    vertices.push_back(min.array() + diag.array() * VectorS<3>(1, 1, 1).array());
    vertices.push_back(min.array() + diag.array() * VectorS<3>(0, 1, 1).array());
    return vertices;
}

std::vector<VectorI<2>> get_edges_mc(const aligned_box<3> &) {
    std::vector<VectorI<2>> edges;
    edges.emplace_back(0, 1);
    edges.emplace_back(1, 2);
    edges.emplace_back(2, 3);
    edges.emplace_back(3, 0);
    edges.emplace_back(4, 5);
    edges.emplace_back(5, 6);
    edges.emplace_back(6, 7);
    edges.emplace_back(7, 4);
    edges.emplace_back(0, 4);
    edges.emplace_back(1, 5);
    edges.emplace_back(2, 6);
    edges.emplace_back(3, 7);
    return edges;
}

Vector<double, 3>
interpolate(double isovalue, const Vector<double, 3> &p1, double sdf1, const Vector<double, 3> &p2, double sdf2) {
    /// Exercise: Please implement the interpolation function for marching cubes
    ///           1. try out what happens if you return one fixed value for each edge, eg. the middle point
    ///           2. use the full interpolation functionality
    if (std::abs(isovalue - sdf1) < scalar_eps) {
        return p1;
    }
    if (std::abs(isovalue - sdf2) < scalar_eps) {
        return p2;
    }
    if (std::abs(sdf1 - sdf2) < scalar_eps) {
        return p1;
    }

    auto t = (isovalue - sdf1) / (sdf2 - sdf1);
    return p1 + t * (p2 - p1);
}

double marching_cubes::hearts_function(const Vector<double, 3> &p) {
    auto x = p(0);
    auto y = p(1);
    auto z = p(2);
    return std::pow(x * x + 9. / 4. * y * y + z * z - 1, 3) - x * x * z * z * z - 9. / 80. * y * y * z * z * z;
}

marching_cubes::marching_cubes() {

}

halfedge_mesh marching_cubes::reconstruct(bcg_scalar_t isovalue, const VectorS<3> &min, const VectorS<3> &max,
                                          const VectorI<3> &dims) {
    if (!implicit_function) return {};
    clear();
    aabb = aligned_box3(min, max);
    this->dims = dims;
    size_t size = capacity();
    halfedge_mesh mesh;
    auto f_normals = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal");
    auto edges = get_edges_mc(aabb);
    Vector<double, 8> sdf;
    for (size_t idx = 0; idx < size; ++idx) {
        aligned_box3 voxel = voxel_bounds(idx_to_coord(idx));

        unsigned int cubeindex = 0;
        auto vertices = get_vetices_mc(voxel);

        for (uint8_t i = 0; i < 8; ++i) {
            /// Exercise: Please implement the computation of the sdf values
            ///           1. dont forget to set the bit values of the cubeindex with SET_BIT(cubeindex, i);
            ///           2. if the surface seems to be inverted, just invert your comparison with the isovalue
            sdf[i] = implicit_function(vertices[i].cast<double>());
            if (sdf[i] >= isovalue) {
                SET_BIT(cubeindex, i);
            }
        }
        if (edge_table[cubeindex] == 0) {
            continue;
        }

        Vector<double, 3> vertex_list[12];
        for (unsigned int i = 0; i < 12; ++i) {
            if (CHECK_BIT(edge_table[cubeindex], i)) {
                auto v0 = edges[i][0];
                auto v1 = edges[i][1];
                vertex_list[i] = interpolate(isovalue,
                                             vertices[v0].cast<double>(), sdf[v0],
                                             vertices[v1].cast<double>(), sdf[v1]);
            }
        }

        for (int j = 0; triangle_table[cubeindex][j] != -1; j += 3) {
            std::vector<vertex_handle> face(3);
            for (int k = 0; k < 3; ++k) {
                face[k] = mesh.add_vertex(vertex_list[triangle_table[cubeindex][j + k]].cast<bcg_scalar_t>());
            }
            mesh.add_face(face);
            f_normals.back() = normal(
                    triangle3(mesh.positions[face[0]], mesh.positions[face[1]], mesh.positions[face[2]]));

        }
    }

    return mesh;
}

void marching_cubes::compute_vertex_normals(halfedge_mesh &mesh) {
    kdtree_property<bcg_scalar_t> index(mesh.positions);
    bcg_scalar_t radius = voxel_side_length().maxCoeff();
    auto v_normal = mesh.vertices.get_or_add<VectorS<3>, 3>("v_normal");
    auto f_normal = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal");

    /// Exercise: Please implement the computation of the vertex normals, remember this is a triangle soup, so you can not use the halfedge_mesh functionality
    ///           1. index is a wrapper for nanoflann kdtree, you can query for points with:
    ///                 -  auto result = index.query_knn(mesh.positions[v], num_closest) for knn search and
    ///                 -  auto result = index.query_radius(mesh.positions[v], radius) for radius search
    ///                 -  descide which one is more suitable for triangle soups
    ///           2. you can iterate over the vertices of a mesh using:
    ///                     for (const auto v : mesh.vertices) {
    ///                         /*your code here*/
    ///                     }
    ///           3. access to the face normals f_normal and vertex normals v_normal properties is provided for you
    ///                 - these are wrappers for std::vector<VectorS<3>> and can be accessed using the face_handle and vertex_handle respectively
    ///           4. mesh is given as a reference, so you only need to write the respective values into v_normal[v]!
    for (const auto v : mesh.vertices) {
        auto result = index.query_radius(mesh.positions[v], radius);
        VectorS<3> vnormal = VectorS<3>::Zero();
        for (const auto idx : result.indices) {
            auto h = mesh.halfedge_graph::get_halfedge(vertex_handle(idx));
            auto f = mesh.get_face(h);
            if (!f.is_valid()) {
                f = mesh.get_face(mesh.get_opposite(h));
            }
            vnormal += f_normal[f];
        }
        v_normal[v] = vnormal.normalized();
    }
    v_normal.set_dirty();
}

}