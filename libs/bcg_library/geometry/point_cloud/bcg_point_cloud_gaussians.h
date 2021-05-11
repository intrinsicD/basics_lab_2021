//
// Created by alex on 06.05.21.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_GAUSSIANS_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_GAUSSIANS_H

#include "bcg_point_cloud.h"
#include "kdtree/bcg_kdtree.h"

namespace bcg{

void point_cloud_gaussians_knn(point_cloud &pc, const kdtree_property<bcg_scalar_t> &index, int num_closest, bool compute_means = true, size_t parallel_grain_size = 1024);

void point_cloud_gaussians_radius(point_cloud &pc, const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius, bool compute_means = true, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_POINT_CLOUD_GAUSSIANS_H
