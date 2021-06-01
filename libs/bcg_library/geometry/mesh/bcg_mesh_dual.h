//
// Created by alex on 31.05.21.
//

#ifndef BCG_GRAPHICS_BCG_MESH_DUAL_H
#define BCG_GRAPHICS_BCG_MESH_DUAL_H

#include "bcg_mesh.h"

namespace bcg{

halfedge_mesh mesh_make_dual(const halfedge_mesh &mesh);

void mesh_dual(halfedge_mesh &mesh);

}

#endif //BCG_GRAPHICS_BCG_MESH_DUAL_H
