//
// Created by alex on 23.02.21.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_VERTEX_NOISE_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_VERTEX_NOISE_H

#include "bcg_point_cloud.h"
#include "aligned_box/bcg_aligned_box.h"

namespace bcg{

void point_cloud_vertex_noise(vertex_container *vertices, bcg_scalar_t sigma_p, size_t parallel_grain_size = 1024);

void point_cloud_vertex_noise(vertex_container *vertices, const aligned_box3 &aabb, bcg_scalar_t noise_level, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_POINT_CLOUD_VERTEX_NOISE_H
