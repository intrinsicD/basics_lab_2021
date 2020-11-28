//
// Created by alex on 27.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_REMESHING_H
#define BCG_GRAPHICS_BCG_MESH_REMESHING_H

#include "bcg_mesh.h"

namespace bcg {

void mesh_remeshing_uniform(halfedge_mesh &mesh, bcg_scalar_t edge_length, unsigned int iterations = 10,
                            bool use_projection = true);

void mesh_remeshing_adaptive(halfedge_mesh &mesh, bcg_scalar_t min_edge_length, bcg_scalar_t max_edge_length,
                             bcg_scalar_t approx_error, unsigned int iterations = 10, bool use_projection = true);

}


#endif //BCG_GRAPHICS_BCG_GUI_MESH_REMESHING_H
