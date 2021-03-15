//
// Created by alex on 06.11.20.
//

#include "geometry/bcg_property.h"
#include "bcg_mesh_vertex_normals.h"
#include "bcg_mesh_face_normals.h"
#include "bcg_mesh_face_area_vector.h"
#include "math/vector/bcg_vector_angle.h"
#include "tbb/tbb.h"

namespace bcg {

VectorS<3> vertex_normal_uniform(halfedge_mesh &mesh, vertex_handle v) {
    VectorS<3> n(zero3s);
    for (const auto f : mesh.get_faces(v)) {
        n += face_normal(mesh, f);
    }
    return n.normalized();
}

VectorS<3> vertex_normal_area(halfedge_mesh &mesh, vertex_handle v) {
    VectorS<3> n(zero3s);
    for (const auto f : mesh.get_faces(v)) {
        n += face_area_vector(mesh, f);
    }
    return n.normalized();
}

VectorS<3> vertex_normal_angle(halfedge_mesh &mesh, vertex_handle v) {
    VectorS<3> n(zero3s);
    for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
        if (mesh.is_boundary(h)) continue;
        auto hr = mesh.rotate_ccw(h);
        n += face_normal(mesh, mesh.get_face(h)) *
             vector_angle<3>(mesh.positions[mesh.get_to_vertex(h)] - mesh.positions[v],
                             mesh.positions[mesh.get_to_vertex(hr)] - mesh.positions[v]);
    }
    return n.normalized();
}

VectorS<3> vertex_normal_area_angle(halfedge_mesh &mesh, vertex_handle v) {
    VectorS<3> n(zero3s);
    for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
        if (mesh.is_boundary(h)) continue;
        auto hr = mesh.rotate_ccw(h);
        n += face_area_vector(mesh, mesh.get_face(h)) *
             vector_angle<3>(mesh.positions[mesh.get_to_vertex(h)] - mesh.positions[v],
                             mesh.positions[mesh.get_to_vertex(hr)] - mesh.positions[v]);
    }
    return n.normalized();
}

void vertex_normals(halfedge_mesh &mesh, std::function<VectorS<3>(halfedge_mesh &, vertex_handle)> method,
                    size_t parallel_grain_size) {
    auto normals = mesh.vertices.get_or_add<VectorS<3>, 3>("v_normal");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    normals[i] = method(mesh, i);
                }
            }
    );
    normals.set_dirty();
}

}