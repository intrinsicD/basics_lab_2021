//
// Created by alex on 19.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_CONNECTED_COMPONENTS_H
#define BCG_GRAPHICS_BCG_MESH_CONNECTED_COMPONENTS_H

#include "bcg_mesh.h"

namespace bcg{

size_t mesh_connected_components_detect(halfedge_mesh &mesh);

std::vector<halfedge_mesh> mesh_connected_components_split(halfedge_mesh &mesh);

}

#endif //BCG_GRAPHICS_BCG_MESH_CONNECTED_COMPONENTS_H
