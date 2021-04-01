//
// Created by alex on 01.04.21.
//

#ifndef BCG_GRAPHICS_BCG_MESH_SPLIT_PATH_H
#define BCG_GRAPHICS_BCG_MESH_SPLIT_PATH_H

#include "bcg_mesh.h"

namespace bcg{

void mesh_remove_path(halfedge_mesh &mesh, property<bcg_scalar_t, 1> path);

std::vector<halfedge_mesh> mesh_split_path(halfedge_mesh &mesh, property<bcg_scalar_t, 1> path);

}

#endif //BCG_GRAPHICS_BCG_MESH_SPLIT_PATH_H
