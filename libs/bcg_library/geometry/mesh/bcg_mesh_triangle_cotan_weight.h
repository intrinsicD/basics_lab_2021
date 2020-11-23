//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_TRIANGLE_COTAN_WEIGHT_H
#define BCG_GRAPHICS_BCG_MESH_TRIANGLE_COTAN_WEIGHT_H

#include "bcg_mesh_triangle_area_from_metric.h"

namespace bcg{

bcg_scalar_t clamp_cot(bcg_scalar_t v);

//compute the cotan value of alpha for edge v0-v1 at v2
bcg_scalar_t triangle_cotan(const VectorS<3> &v0, const VectorS<3> &v1, const VectorS<3> &v2);

}

#endif //BCG_GRAPHICS_BCG_MESH_TRIANGLE_COTAN_WEIGHT_H
