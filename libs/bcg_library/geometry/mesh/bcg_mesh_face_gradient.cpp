//
// Created by alex on 23.11.20.
//

#include "bcg_mesh_face_gradient.h"
#include "bcg_mesh_face_normals.h"
#include "bcg_mesh_face_area_vector.h"

namespace bcg{

VectorS<3> face_gradient(const halfedge_mesh &mesh, face_handle f, property<bcg_scalar_t , 1> vproperty) {
    VectorS<3> grad(zero3s);
    for (const auto h : mesh.get_halfedges(f)) {
        auto ev = mesh.positions[mesh.get_to_vertex(h)] - mesh.positions[mesh.get_from_vertex(h)];
        grad += face_normal(mesh, f).cross(ev) * vproperty[mesh.get_to_vertex(mesh.get_next(h))];
    }
    return grad / (2.0 * face_area_vector(mesh, f).norm());
}

}