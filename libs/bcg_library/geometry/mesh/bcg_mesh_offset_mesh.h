//
// Created by alex on 13.04.21.
//

#ifndef BCG_GRAPHICS_BCG_MESH_OFFSET_MESH_H
#define BCG_GRAPHICS_BCG_MESH_OFFSET_MESH_H

#include "bcg_mesh.h"

namespace bcg{

void mesh_normal_offset_mesh(halfedge_mesh &mesh, bcg_scalar_t offset);

void mesh_face_normal_offset_mesh(halfedge_mesh &mesh, bcg_scalar_t offset, bcg_scalar_t sigma_p, size_t parallel_grain_size = 1024);



}

#endif //BCG_GRAPHICS_BCG_MESH_OFFSET_MESH_H
