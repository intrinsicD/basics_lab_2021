//
// Created by alex on 06.11.20.
//

#include "bcg_mesh_face_normals.h"
#include "bcg_mesh_face_area_vector.h"
#include "tbb/tbb.h"

namespace bcg{

VectorS<3> face_normal(const halfedge_mesh &mesh, face_handle f) {
    return face_area_vector(mesh, f).normalized();
}

void face_normals(halfedge_mesh &mesh, size_t parallel_grain_size){
    auto normals = mesh.faces.get_or_add<VectorS<3>, 3>("f_normal");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    normals[i] = face_normal(mesh, i);
                }
            }
    );
    normals.set_dirty();
}

}