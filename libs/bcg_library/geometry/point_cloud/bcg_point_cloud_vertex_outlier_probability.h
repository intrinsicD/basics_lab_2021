//
// Created by alex on 03.12.20.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_VERTEX_OUTLIER_PROBABILITY_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_VERTEX_OUTLIER_PROBABILITY_H

#include "bcg_point_cloud.h"
#include "kdtree/bcg_kdtree.h"

namespace bcg{

void point_cloud_vertex_outlier_probability_knn(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index, int num_closest, size_t parallel_grain_size = 1024);

void point_cloud_vertex_outlier_probability_radius(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius, size_t parallel_grain_size = 1024);

void point_cloud_vertex_remove_outliers(point_cloud &pc, bcg_scalar_t threshold, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_POINT_CLOUD_VERTEX_OUTLIER_PROBABILITY_H
