//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_FACE_NORMALS_H
#define BCG_GRAPHICS_BCG_MESH_FACE_NORMALS_H

#include "bcg_mesh.h"

namespace bcg {

VectorS<3> face_normal(const halfedge_mesh &mesh, face_handle f);

void face_normals(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_FACE_NORMALS_H
