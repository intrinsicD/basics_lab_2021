//
// Created by alex on 01.03.21.
//

#ifndef BCG_GRAPHICS_BCG_MESH_NORMAL_FILTERING_H
#define BCG_GRAPHICS_BCG_MESH_NORMAL_FILTERING_H

#include "geometry/mesh/bcg_mesh.h"

namespace bcg{

struct mesh_nf_parameters{
    bcg_scalar_t sigma_f;
    bcg_scalar_t sigma_g;
    int iterations;
    int something_else;
};

void mesh_normal_filtering(halfedge_mesh &mesh, const mesh_nf_parameters &params, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_NORMAL_FILTERING_H
