//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_EDGE_FUJIWARA_H
#define BCG_GRAPHICS_BCG_MESH_EDGE_FUJIWARA_H

#include "bcg_mesh.h"

namespace bcg{

bcg_scalar_t edge_fujiwara(const halfedge_mesh &mesh, edge_handle e);

void edge_fujiwaras(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

}


#endif //BCG_GRAPHICS_BCG_MESH_EDGE_FUJIWARA_H
