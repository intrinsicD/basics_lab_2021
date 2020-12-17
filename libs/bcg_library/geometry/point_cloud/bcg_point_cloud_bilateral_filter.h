//
// Created by alex on 14.12.20.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_BILATERAL_FILTER_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_BILATERAL_FILTER_H


#include "bcg_point_cloud.h"
#include "kdtree/bcg_kdtree.h"

namespace bcg {

struct bilateral_filter_params {
    bcg_scalar_t points_sigma, normals_sigma;
};

void point_cloud_bilateral_filter_knn(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index,
                                      const bilateral_filter_params &params, int num_closest = 3,
                                      size_t parallel_grain_size = 1024);

void point_cloud_bilateral_filter_radius(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index,
                                         const bilateral_filter_params &params, bcg_scalar_t radius = 0.001,
                                         size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_POINT_CLOUD_BILATERAL_FILTER_H
