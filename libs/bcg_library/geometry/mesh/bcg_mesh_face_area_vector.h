//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_FACE_AREA_VECTOR_H
#define BCG_GRAPHICS_BCG_MESH_FACE_AREA_VECTOR_H

#include "bcg_mesh.h"

namespace bcg{

inline VectorS<3> face_area_vector(const halfedge_mesh &mesh, face_handle f) {
    VectorS<3> vector_area = zero3s;
    for (const auto h : mesh.get_halfedges(f)) {
        vector_area += mesh.positions[mesh.get_from_vertex(h)].cross(mesh.positions[mesh.get_to_vertex(h)]) / 2;
    }
    return vector_area;
}

}

#endif //BCG_GRAPHICS_BCG_MESH_FACE_AREA_VECTOR_H
