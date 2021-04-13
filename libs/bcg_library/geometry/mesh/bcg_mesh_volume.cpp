//
// Created by alex on 13.04.21.
//

#include "bcg_mesh_volume.h"
#include "bcg_property_map_eigen.h"
#include "tbb/tbb.h"

namespace bcg {

/* Paper: EFFICIENT FEATURE EXTRACTION FOR 2D/3D OBJECTS IN MESH REPRESENTATION
 *  volume computations
 *
 * */

bcg_scalar_t mesh_volume(halfedge_mesh &mesh, face_handle f) {
    std::vector<VectorS<3>> V;
    for (const auto vf : mesh.get_vertices(f)) {
        V.push_back(mesh.positions[vf]);
    }
    bcg_scalar_t v321 = V[2][0] * V[1][1] * V[0][2];
    bcg_scalar_t v231 = V[1][0] * V[2][1] * V[0][2];
    bcg_scalar_t v312 = V[2][0] * V[0][1] * V[1][2];
    bcg_scalar_t v132 = V[0][0] * V[2][1] * V[1][2];
    bcg_scalar_t v213 = V[1][0] * V[0][1] * V[2][2];
    bcg_scalar_t v123 = V[0][0] * V[1][1] * V[2][2];

    return (-v321 + v231 + v312 - v132 - v213 + v123) / 6.0;
}

bcg_scalar_t mesh_volume(halfedge_mesh &mesh, size_t parallel_grain_size) {
    auto signed_tet_volume = mesh.faces.get_or_add<bcg_scalar_t, 1>("f_signed_tet_volume");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.faces.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto f = face_handle(i);
                    signed_tet_volume[f] = mesh_volume(mesh, f);
                }
            }
    );
    signed_tet_volume.set_dirty();
    return std::abs(MapConst(signed_tet_volume).sum());
}

}