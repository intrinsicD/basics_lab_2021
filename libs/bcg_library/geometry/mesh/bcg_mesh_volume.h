//
// Created by alex on 13.04.21.
//

#ifndef BCG_GRAPHICS_BCG_MESH_VOLUME_H
#define BCG_GRAPHICS_BCG_MESH_VOLUME_H

#include "bcg_mesh.h"

namespace bcg{

bcg_scalar_t mesh_volume(halfedge_mesh &mesh, face_handle f);

bcg_scalar_t mesh_volume(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_VOLUME_H
