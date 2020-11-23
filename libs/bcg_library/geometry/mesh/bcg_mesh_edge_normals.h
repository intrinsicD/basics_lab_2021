//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_EDGE_NORMALS_H
#define BCG_GRAPHICS_BCG_MESH_EDGE_NORMALS_H

#include "bcg_mesh.h"

namespace bcg{

VectorS<3> edge_normal(const halfedge_mesh &mesh, edge_handle e);

void edge_normals(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_EDGE_NORMALS_H
