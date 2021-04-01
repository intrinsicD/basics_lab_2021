//
// Created by alex on 19.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_VERTEX_CONVEX_CONCAVE_H
#define BCG_GRAPHICS_BCG_MESH_VERTEX_CONVEX_CONCAVE_H

#include "bcg_mesh.h"

namespace bcg{

void vertex_convex_concave(halfedge_mesh &mesh, int post_smoothing_steps = 3, bool two_ring_neighborhood = true, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_VERTEX_CONVEX_CONCAVE_H
