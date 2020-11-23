//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_VERTEX_FUJIWARA_H
#define BCG_GRAPHICS_BCG_MESH_VERTEX_FUJIWARA_H

#include "bcg_mesh.h"

namespace bcg{

bcg_scalar_t vertex_fujiwara(halfedge_mesh &mesh, vertex_handle v);

void vertex_fujiwara(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_VERTEX_FUJIWARA_H
