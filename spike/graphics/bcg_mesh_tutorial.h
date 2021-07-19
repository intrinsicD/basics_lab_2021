//
// Created by alex on 01.03.21.
//

#ifndef BCG_GRAPHICS_BCG_MESH_TUTORIAL_H
#define BCG_GRAPHICS_BCG_MESH_TUTORIAL_H

#include "geometry/mesh/bcg_mesh.h"

namespace bcg{

struct mesh_tutorial_parameters{
    bcg_scalar_t parameter_1;
    bcg_scalar_t parameter_2;
    int iterations;
    int something_else;
};

void mesh_geodesic_approx(halfedge_mesh &mesh, const mesh_tutorial_parameters &params, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_MESH_TUTORIAL_H
