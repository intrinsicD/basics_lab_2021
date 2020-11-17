//
// Created by alex on 16.11.20.
//

#include "bcg_mesh_face_centers.h"
#include "tbb/tbb.h"

namespace bcg{

VectorS<3> face_center(const halfedge_mesh &mesh, face_handle f){
    VectorS<3> center = zero3s;
    size_t count = 0;
    for (const auto v : mesh.get_vertices(f)) {
        center += mesh.positions[v];
        ++count;
    }
    return center / bcg_scalar_t(count);
}

void face_centers(halfedge_mesh &mesh, size_t parallel_grain_size){
    auto position = mesh.faces.get_or_add<VectorS<3>, 3>("f_position");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    position[i] = face_center(mesh, i);
                }
            }
    );
    position.set_dirty();
}

}