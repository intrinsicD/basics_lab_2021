//
// Created by alex on 24.02.21.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_NORMAL_FILTERING_ROBUST_STATISTICS_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_NORMAL_FILTERING_ROBUST_STATISTICS_H

#include "bcg_point_cloud.h"
#include "kdtree/bcg_kdtree.h"

namespace bcg {

enum class PointCloudNormalFilteringType {
    averaging_knn,
    averaging_radius,
    __last__
};

std::vector<std::string> point_cloud_normal_filtering_type_names();

void
point_cloud_normal_filtering_robust_statistics_knn(vertex_container *vertices, kdtree_property<bcg_scalar_t> &index,
                                                   int num_closest, bcg_scalar_t sigma_g, bcg_scalar_t sigma_p,
                                                   bcg_scalar_t sigma_n, bcg_scalar_t lambda,
                                                   bool use_repulsion_tangential, bool use_attraction_normal,
                                                   size_t parallel_grain_size = 1024);

void
point_cloud_normal_filtering_robust_statistics_radius(vertex_container *vertices, kdtree_property<bcg_scalar_t> &index,
                                                      bcg_scalar_t radius, bcg_scalar_t sigma_p, bcg_scalar_t sigma_n,
                                                      size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_POINT_CLOUD_NORMAL_FILTERING_ROBUST_STATISTICS_H
