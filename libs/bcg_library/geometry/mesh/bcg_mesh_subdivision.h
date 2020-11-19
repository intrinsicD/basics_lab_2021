//
// Created by alex on 19.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_SUBDIVISION_H
#define BCG_GRAPHICS_BCG_MESH_SUBDIVISION_H

#include "bcg_mesh.h"

namespace bcg{

void mesh_subdivision_catmull_clark(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

void mesh_subdivision_loop(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

void mesh_subdivision_sqrt3(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_SUBDIVISION_H
