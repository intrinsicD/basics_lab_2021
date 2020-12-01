//
// Created by alex on 01.12.20.
//

#ifndef BCG_GRAPHICS_BCG_VERTEX_CLASSIFY_CURVATURE_H
#define BCG_GRAPHICS_BCG_VERTEX_CLASSIFY_CURVATURE_H

#include "geometry/point_cloud/bcg_point_cloud.h"

namespace bcg{

void vertex_classify_curvature(vertex_container *vertices, property<bcg_scalar_t, 1> min_curvature,
                               property<bcg_scalar_t, 1> max_curvature, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_VERTEX_CLASSIFY_CURVATURE_H
