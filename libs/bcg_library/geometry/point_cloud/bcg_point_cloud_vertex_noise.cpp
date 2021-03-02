//
// Created by alex on 23.02.21.
//

#include "bcg_point_cloud_vertex_noise.h"
#include "tbb/tbb.h"

namespace bcg {

void point_cloud_vertex_noise(vertex_container *vertices, bcg_scalar_t sigma_p, size_t parallel_grain_size) {
    auto positions = vertices->get<VectorS<3>, 3>("v_position");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    positions[v] += VectorS<3>::Random() * sigma_p;
                }
            }
    );
    positions.set_dirty();
}

void point_cloud_vertex_noise(vertex_container *vertices, const aligned_box3 &aabb, bcg_scalar_t noise_level,
                              size_t parallel_grain_size) {
    bcg_scalar_t sigma_p = aabb.diagonal().norm() * noise_level / bcg_scalar_t(100);
    point_cloud_vertex_noise(vertices, sigma_p, parallel_grain_size);
}

}