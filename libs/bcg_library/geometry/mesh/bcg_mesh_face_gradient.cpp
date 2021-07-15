//
// Created by alex on 23.11.20.
//

#include "bcg_mesh_face_gradient.h"
#include "bcg_mesh_face_normals.h"
#include "bcg_mesh_face_area_vector.h"
#include "tbb/tbb.h"

namespace bcg{

VectorS<3> face_gradient(const halfedge_mesh &mesh, face_handle f, property<bcg_scalar_t , 1> vproperty) {
    VectorS<3> grad(zero3s);
    bcg_scalar_t mag = 0;
    for (const auto h : mesh.get_halfedges(f)) {
        mag = std::max(mag, vproperty[mesh.get_to_vertex(mesh.get_next(h))]);
    }
    mag = 1.0 / mag;
    for (const auto h : mesh.get_halfedges(f)) {
        auto ev = mesh.positions[mesh.get_to_vertex(h)] - mesh.positions[mesh.get_from_vertex(h)];
        grad += face_normal(mesh, f).cross(ev) * vproperty[mesh.get_to_vertex(mesh.get_next(h))] * mag;
    }
    return grad / (2.0 * face_area_vector(mesh, f).norm());
}

property<VectorS<3>, 3> face_gradients(halfedge_mesh &mesh, property<bcg_scalar_t , 1> vproperty,size_t parallel_grain_size){
    auto gradient = mesh.faces.get_or_add<VectorS<3>, 3>("f_gradient");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    gradient[i] = face_gradient(mesh, i, vproperty);
                }
            }
    );
    gradient.set_dirty();
    return gradient;
}

}