//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_VERTEX_VALENCES_H
#define BCG_GRAPHICS_BCG_MESH_VERTEX_VALENCES_H

#include "bcg_mesh.h"

namespace bcg{

void vertex_valences(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_VERTEX_VALENCES_H
