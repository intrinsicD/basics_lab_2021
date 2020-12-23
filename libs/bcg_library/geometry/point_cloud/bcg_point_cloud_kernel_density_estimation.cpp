//
// Created by alex on 03.12.20.
//

#include "bcg_point_cloud_kernel_density_estimation.h"
#include "sphere/bcg_sphere.h"
#include "math/vector/bcg_vector_map_eigen.h"
#include "math/statistics/bcg_gaussian.h"
#include "tbb/tbb.h"

namespace bcg {

void
point_cloud_kernel_density_estimation_knn(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index, int num_closest, size_t parallel_grain_size) {
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto kernel_density = vertices->get_or_add<bcg_scalar_t, 1>("v_kernel_density");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    auto result = index.query_knn(positions[v], num_closest);
                    bcg_scalar_t V = sphere<3>(Map(result.distances).maxCoeff()).volume();
                    kernel_density[i] = bcg_scalar_t(num_closest) / (vertices->size() * V);
                }
            }
    );
    kernel_density.set_dirty();
}

void point_cloud_kernel_density_estimation_radius(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius,
                                                  size_t parallel_grain_size) {
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto kernel_density = vertices->get_or_add<bcg_scalar_t, 1>("v_kernel_density");
    bcg_scalar_t sigma_squared = radius * radius;
    bcg_scalar_t normalizer = gaussian_normalizer(sigma_squared);
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);

                    auto result = index.query_radius(positions[v], radius);
                    kernel_density[v] = 0;
                    for(const auto &distance : result.distances){
                        kernel_density[v] += gaussian(distance * distance, sigma_squared) / (normalizer * vertices->size());
                    }
                }
            }
    );
    kernel_density.set_dirty();
}


}