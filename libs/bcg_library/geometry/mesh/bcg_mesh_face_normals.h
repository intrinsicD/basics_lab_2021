//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_FACE_NORMALS_H
#define BCG_GRAPHICS_BCG_MESH_FACE_NORMALS_H

#include "bcg_mesh_face_area_vector.h"

namespace bcg{

inline VectorS<3> face_normal(const halfedge_mesh &mesh, face_handle f) {
    return face_area_vector(mesh, f).normalized();
}

}

#endif //BCG_GRAPHICS_BCG_MESH_FACE_NORMALS_H
