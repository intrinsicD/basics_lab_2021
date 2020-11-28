//
// Created by alex on 27.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_SIMPLIFICATION_H
#define BCG_GRAPHICS_BCG_MESH_SIMPLIFICATION_H

#include "bcg_mesh.h"

namespace bcg {

void mesh_simplification(halfedge_mesh &mesh, unsigned int n_vertices, bcg_scalar_t aspect_ratio = 0.0,
                         bcg_scalar_t edge_length = 0.0, unsigned int max_valence = 0,
                         bcg_scalar_t normal_deviation = 0.0, bcg_scalar_t hausdorff_error = 0.0);

}

#endif //BCG_GRAPHICS_BCG_MESH_SIMPLIFICATION_H
