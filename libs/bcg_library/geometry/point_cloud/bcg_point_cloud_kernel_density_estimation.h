//
// Created by alex on 03.12.20.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_KERNEL_DENSITY_ESTIMATION_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_KERNEL_DENSITY_ESTIMATION_H

#include "bcg_point_cloud.h"
#include "kdtree/bcg_kdtree.h"

namespace bcg{

void point_cloud_kernel_density_estimation_knn(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index, int num_closest, size_t parallel_grain_size = 1024);

void point_cloud_kernel_density_estimation_radius(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_POINT_CLOUD_KERNEL_DENSITY_ESTIMATION_H
