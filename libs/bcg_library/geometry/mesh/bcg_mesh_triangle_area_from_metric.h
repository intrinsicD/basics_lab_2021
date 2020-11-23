//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_TRIANGLE_AREA_FROM_METRIC_H
#define BCG_GRAPHICS_BCG_MESH_TRIANGLE_AREA_FROM_METRIC_H

#include "geometry/triangle/bcg_triangle.h"

namespace bcg{

bcg_scalar_t triangle_area_from_metric(bcg_scalar_t a, bcg_scalar_t b, bcg_scalar_t c, bcg_scalar_t eps = scalar_eps);

bcg_scalar_t triangle_area_from_metric(const VectorS<3> &abc, bcg_scalar_t eps = scalar_eps);

template<int N>
inline bcg_scalar_t triangle_area_from_metric(const triangle<N> &triangle, bcg_scalar_t eps = scalar_eps) {
    return triangle_area_from_metric(triangle.edge_lengths(), eps);
}

}

#endif //BCG_GRAPHICS_BCG_MESH_TRIANGLE_AREA_FROM_METRIC_H
