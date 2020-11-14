//
// Created by alex on 14.11.20.
//

#include "bcg_mesh_edge_dihedral_angle.h"
#include "bcg_mesh_face_normals.h"
#include "math/vector/bcg_math_vector_angle.h"
#include "tbb/tbb.h"

namespace bcg {

bcg_scalar_t edge_dihedral_angle(halfedge_mesh &mesh, edge_handle e) {
    auto f0 = mesh.get_face(e, 0);
    auto f1 = mesh.get_face(e, 1);
    if (mesh.faces.is_valid(f0) && mesh.faces.is_valid(f1)) {
        return vector_angle(face_normal(mesh, f0), face_normal(mesh, f1));
    }
    return 0;
}

void edge_dihedral_angles(halfedge_mesh &mesh, size_t parallel_grain_size) {
    auto dihedral_angles = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_dihedral_angle");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    dihedral_angles[i] = edge_dihedral_angle(mesh, i);
                }
            }
    );
    dihedral_angles.set_dirty();
}


}