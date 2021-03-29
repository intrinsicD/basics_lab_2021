//
// Created by alex on 29.03.21.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_VERTEX_SALIENCY_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_VERTEX_SALIENCY_H

#include "bcg_point_cloud.h"
#include "kdtree/bcg_kdtree.h"

namespace bcg {

void point_cloud_vertex_saliency(vertex_container *vertices, property<bcg_scalar_t, 1> property,
                                 const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_POINT_CLOUD_VERTEX_SALIENCY_H
