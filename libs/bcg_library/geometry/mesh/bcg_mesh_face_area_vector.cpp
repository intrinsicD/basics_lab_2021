//
// Created by alex on 06.11.20.
//

#include "bcg_mesh_face_area_vector.h"

namespace bcg{

VectorS<3> face_area_vector(const halfedge_mesh &mesh, face_handle f) {
    VectorS<3> vector_area = zero3s;
    for (const auto h : mesh.get_halfedges(f)) {
        vector_area += mesh.positions[mesh.get_from_vertex(h)].cross(mesh.positions[mesh.get_to_vertex(h)]) / 2;
    }
    return vector_area;
}

}