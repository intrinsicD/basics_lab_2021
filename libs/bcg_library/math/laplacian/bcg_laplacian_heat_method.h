//
// Created by alex on 13.07.21.
//

#ifndef BCG_GRAPHICS_BCG_LAPLACIAN_HEAT_METHOD_H
#define BCG_GRAPHICS_BCG_LAPLACIAN_HEAT_METHOD_H


#include "mesh/bcg_mesh.h"
#include "bcg_laplacian_matrix.h"

namespace bcg {

bcg_scalar_t compute_t_squared(halfedge_mesh &mesh, bcg_scalar_t m = 1);

property<bcg_scalar_t, 1> heat_method(halfedge_mesh &mesh, const laplacian_matrix &laplacian, bcg_scalar_t timestep, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_LAPLACIAN_HEAT_METHOD_H
