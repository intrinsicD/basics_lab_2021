//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_FACE_CENTERS_H
#define BCG_GRAPHICS_BCG_MESH_FACE_CENTERS_H

#include "bcg_mesh.h"

namespace bcg {

VectorS<3> face_center(const halfedge_mesh &mesh, face_handle f);

void face_centers(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

}


#endif //BCG_GRAPHICS_BCG_MESH_FACE_CENTERS_H
