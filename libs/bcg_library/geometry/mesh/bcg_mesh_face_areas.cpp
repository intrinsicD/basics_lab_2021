//
// Created by alex on 28.11.20.
//

#include "bcg_mesh_face_areas.h"
#include "bcg_mesh_face_area_vector.h"
#include "tbb/tbb.h"

namespace bcg{

bcg_scalar_t face_area(halfedge_mesh &mesh, face_handle f){
    return std::max(face_area_vector(mesh, f).norm(), scalar_eps);
}

void face_areas(halfedge_mesh &mesh, size_t parallel_grain_size){
    auto areas = mesh.faces.get_or_add<bcg_scalar_t, 1>("f_area");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    areas[f] = face_area(mesh, f);
                }
            }
    );
    areas.set_dirty();
}

}