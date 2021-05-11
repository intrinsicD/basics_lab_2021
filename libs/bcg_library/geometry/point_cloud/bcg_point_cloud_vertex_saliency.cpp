//
// Created by alex on 29.03.21.
//

#include "bcg_point_cloud_vertex_saliency.h"
#include "math/statistics/bcg_gaussian.h"
#include "tbb/tbb.h"

namespace bcg {

void point_cloud_vertex_saliency(vertex_container *vertices, property<bcg_scalar_t, 1> property,
                                 const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius,
                                 size_t parallel_grain_size) {
    if(!property) return;
    auto diff = vertices->get_or_add<bcg_scalar_t, 1>("v_saliency_diff");
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto scale_small = vertices->get_or_add<bcg_scalar_t, 1>("v_saliency_scale_small");
    auto scale_large = vertices->get_or_add<bcg_scalar_t, 1>("v_saliency_scale_large");
    bcg_scalar_t two_times_radius = 2.0 * radius;
    bcg_scalar_t sigma_small_squared = radius * radius;
    bcg_scalar_t sigma_large_squared = two_times_radius * two_times_radius;

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    auto result = index.query_radius(positions[v], two_times_radius);

                    bcg_scalar_t sum_weights_small = 0;
                    bcg_scalar_t value_small = 0;

                    bcg_scalar_t sum_weights_large = 0;
                    bcg_scalar_t value_large = 0;
                    for (size_t j = 0; j < result.indices.size(); ++j) {
                        size_t idx = result.indices[j];
                        bcg_scalar_t distance_squared = result.distances[j] * result.distances[j];
                        bcg_scalar_t weight_large = gaussian_distance(distance_squared, sigma_large_squared);
                        value_large += property[idx] * weight_large;
                        sum_weights_large += weight_large;

                        if(result.distances[j] < radius){
                            bcg_scalar_t weight_small = gaussian_distance(distance_squared, sigma_small_squared);
                            value_small += property[idx] * weight_small;
                            sum_weights_small += weight_small;
                        }
                    }
                    value_large /= sum_weights_large;
                    scale_large[v] = value_large;

                    value_small /= sum_weights_small;
                    scale_small[v] = value_small;

                    diff[v] = scale_large[v] - scale_small[v];
                }
            }
    );
    diff.set_dirty();
    scale_small.set_dirty();
    scale_large.set_dirty();
}

}