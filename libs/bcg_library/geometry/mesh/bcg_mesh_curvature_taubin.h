//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_CURVATURE_TAUBIN_H
#define BCG_GRAPHICS_BCG_MESH_CURVATURE_TAUBIN_H

#include "bcg_mesh.h"

namespace bcg{

void mesh_curvature_taubin(halfedge_mesh &mesh, int post_smoothing_steps = 3, bool two_ring_neighborhood = true, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_CURVATURE_TAUBIN_H
