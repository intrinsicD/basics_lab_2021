//
// Created by alex on 13.07.21.
//

#ifndef BCG_GRAPHICS_BCG_MESH_VERTEX_DIVERGENCE_H
#define BCG_GRAPHICS_BCG_MESH_VERTEX_DIVERGENCE_H

#include "bcg_mesh.h"

namespace bcg{

bcg_scalar_t vertex_divergence(halfedge_mesh &mesh, vertex_handle v, property<VectorS<3>, 3> fgradient);

property<bcg_scalar_t, 1> vertex_divergences(halfedge_mesh &mesh, property<VectorS<3>, 3> fgradient, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_VERTEX_DIVERGENCE_H
