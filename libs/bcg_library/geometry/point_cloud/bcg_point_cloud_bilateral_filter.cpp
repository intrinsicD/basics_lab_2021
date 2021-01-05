//
// Created by alex on 14.12.20.
//

#include "bcg_point_cloud_bilateral_filter.h"
#include "bcg_property_map_eigen.h"
#include "tbb/tbb.h"

namespace bcg {

/* Paper: The Bilateral Filter for Point Clouds, Julie Digne, Carlo de Franchis
 * */

void point_cloud_bilateral_filter_knn(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index,
                                      const bilateral_filter_params &params,
                                      int num_closest,
                                      size_t parallel_grain_size) {
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto new_positions = vertices->get_or_add<VectorS<3>, 3>("v_bilateral_smoothed_position");
    auto difference_vectors = vertices->get_or_add<VectorS<3>, 3>("v_bilateral_vector");

    auto delta = vertices->get_or_add<bcg_scalar_t, 1>("v_bilateral_delta");
    auto normals = vertices->get<VectorS<3>, 3>("v_normal");
    if (!normals || MapConst(normals).sum() == 0) {
        if (!vertices->has("v_pca_normal")) {
            std::cout << "please compute vertex pca first!\n";
            return;
        }
        normals = vertices->get<VectorS<3>, 3>("v_pca_normal");
    }

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    auto result = index.query_knn(positions[v], num_closest);
                    delta[v] = 0;
                    bcg_scalar_t sum = 0;
                    for (int j = 0; j < num_closest; ++j) {
                        VectorS<3> diff = positions[result.indices[j]] - positions[v];
                        bcg_scalar_t d_d = diff.squaredNorm();
                        bcg_scalar_t d_n = diff.dot(normals[v]);
                        bcg_scalar_t w =
                                std::exp(-d_d / (2 * params.points_sigma * params.points_sigma)) *
                                std::exp(-d_n * d_n  / (2 * params.normals_sigma * params.normals_sigma));
                        delta[v] += w * d_n;
                        sum += w;
                    }
                    new_positions[v] = positions[v] + delta[v] * normals[v] / sum;
                    difference_vectors[v] = new_positions[v] - positions[v];
                }
            }
    );

    delta.set_dirty();
    new_positions.set_dirty();
    difference_vectors.set_dirty();
}

void point_cloud_bilateral_filter_radius(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index,
                                         const bilateral_filter_params &params,
                                         bcg_scalar_t radius,
                                         size_t parallel_grain_size) {
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto new_positions = vertices->get_or_add<VectorS<3>, 3>("v_bilateral_smoothed_position");
    auto difference_vectors = vertices->get_or_add<VectorS<3>, 3>("v_bilateral_vector");

    auto delta = vertices->get_or_add<bcg_scalar_t, 1>("v_bilateral_delta");
    auto normals = vertices->get<VectorS<3>, 3>("v_normal");
    if (!normals || MapConst(normals).sum() == 0) {
        if (!vertices->has("v_pca_normal")) {
            std::cout << "please compute vertex pca first!\n";
            return;
        }
        normals = vertices->get<VectorS<3>, 3>("v_pca_normal");
    }

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    auto result = index.query_radius(positions[v], radius);
                    delta[v] = 0;
                    bcg_scalar_t sum = 0;
                    for (int j = 0; j < result.indices.size(); ++j) {
                        VectorS<3> diff = positions[result.indices[j]] - positions[v];
                        bcg_scalar_t d_d = diff.squaredNorm();
                        bcg_scalar_t d_n = diff.dot(normals[v]);
                        bcg_scalar_t w =
                                std::exp(-d_d / (2 * params.points_sigma * params.points_sigma)) *
                                std::exp(-d_n * d_n / (2 * params.normals_sigma * params.normals_sigma));
                        delta[v] += w * d_n;
                        sum += w;
                    }
                    new_positions[v] = positions[v] + delta[v] * normals[v] / sum;
                    difference_vectors[v] = new_positions[v] - positions[v];
                }
            }
    );

    delta.set_dirty();
    new_positions.set_dirty();
    difference_vectors.set_dirty();
}
}