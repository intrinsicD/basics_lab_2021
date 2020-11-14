//
// Created by alex on 14.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_EDGE_DIHEDRAL_ANGLE_H
#define BCG_GRAPHICS_BCG_MESH_EDGE_DIHEDRAL_ANGLE_H

#include "bcg_mesh.h"

namespace bcg{

bcg_scalar_t edge_dihedral_angle(halfedge_mesh &mesh, edge_handle e);

void edge_dihedral_angles(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_EDGE_DIHEDRAL_ANGLE_H
