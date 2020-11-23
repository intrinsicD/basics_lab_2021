//
// Created by alex on 23.11.20.
//

#include "bcg_mesh_edge_normals.h"
#include "bcg_mesh_face_normals.h"
#include "tbb/tbb.h"

namespace bcg{

VectorS<3> edge_normal(const halfedge_mesh &mesh, edge_handle e) {
    auto f0 = mesh.get_face(e, 0);
    auto f1 = mesh.get_face(e, 1);
    return (face_normal(mesh, f0) + face_normal(mesh, f1)).normalized();
}

void edge_normals(halfedge_mesh &mesh, size_t parallel_grain_size){
    auto normals = mesh.edges.get_or_add<VectorS<3>, 3>("e_normal");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    normals[i] = edge_normal(mesh, i);
                }
            }
    );
    normals.set_dirty();
}

}