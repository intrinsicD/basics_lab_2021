//
// Created by alex on 15.04.21.
//

#ifndef BCG_GRAPHICS_BCG_MESH_PROJECTED_DISTANCES_H
#define BCG_GRAPHICS_BCG_MESH_PROJECTED_DISTANCES_H

#include "bcg_mesh.h"

namespace bcg{

void mesh_projected_distances(halfedge_mesh &source, halfedge_mesh &target, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_PROJECTED_DISTANCES_H
