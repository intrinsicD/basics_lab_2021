//
// Created by alex on 01.12.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_POINT_CLOUD_CURVATURE_TAUBIN_H
#define BCG_GRAPHICS_BCG_GUI_POINT_CLOUD_CURVATURE_TAUBIN_H

#include "bcg_point_cloud.h"
#include "kdtree/bcg_kdtree.h"

namespace bcg{

void point_cloud_curvature_taubin(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index , int num_closest = 3, size_t parallel_grain_size = 1024);

void point_cloud_curvature_taubin(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index , bcg_scalar_t radius = 0.001, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_GUI_POINT_CLOUD_CURVATURE_TAUBIN_H
