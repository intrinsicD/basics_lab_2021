//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_VERTEX_NORMALS_H
#define BCG_GRAPHICS_BCG_MESH_VERTEX_NORMALS_H

#include "bcg_mesh_face_normals.h"
#include "math/vector/bcg_math_vector_angle.h"

namespace bcg{

inline VectorS<3> vertex_normal_uniform(halfedge_mesh &mesh, vertex_handle v){
    VectorS<3> n(zero3s);
    for (const auto f : mesh.get_faces(v)) {
        n += face_normal(mesh, f);
    }
    return n.normalized();
}

inline VectorS<3> vertex_normal_area(halfedge_mesh &mesh, vertex_handle v){
    VectorS<3> n(zero3s);
    for (const auto f : mesh.get_faces(v)) {
        n += face_area_vector(mesh, f);
    }
    return n.normalized();
}

inline VectorS<3> vertex_normal_angle(halfedge_mesh &mesh, vertex_handle v){
    VectorS<3> n(zero3s);
    for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
        if(mesh.is_boundary(h)) continue;
        auto hr = mesh.rotate_ccw(h);
        n += face_normal(mesh, mesh.get_face(h)) * vector_angle<3>(mesh.positions[mesh.get_to_vertex(h)] - mesh.positions[v], mesh.positions[mesh.get_to_vertex(hr)] - mesh.positions[v]);
    }
    return n.normalized();
}

inline VectorS<3> vertex_normal_area_angle(halfedge_mesh &mesh, vertex_handle v){
    VectorS<3> n(zero3s);
    for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
        if(mesh.is_boundary(h)) continue;
        auto hr = mesh.rotate_ccw(h);
        n += face_area_vector(mesh, mesh.get_face(h)) * vector_angle<3>(mesh.positions[mesh.get_to_vertex(h)] - mesh.positions[v], mesh.positions[mesh.get_to_vertex(hr)] - mesh.positions[v]);
    }
    return n.normalized();
}

}

#endif //BCG_GRAPHICS_BCG_MESH_VERTEX_NORMALS_H
