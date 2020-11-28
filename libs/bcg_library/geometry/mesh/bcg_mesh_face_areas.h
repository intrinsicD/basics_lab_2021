//
// Created by alex on 28.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_FACE_AREAS_H
#define BCG_GRAPHICS_BCG_MESH_FACE_AREAS_H

#include "bcg_mesh.h"

namespace bcg{

void face_areas(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_FACE_AREAS_H
