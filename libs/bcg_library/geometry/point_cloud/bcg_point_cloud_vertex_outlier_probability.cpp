//
// Created by alex on 03.12.20.
//

#include "bcg_point_cloud_vertex_outlier_probability.h"
#include "math/vector/bcg_vector_map_eigen.h"
#include "bcg_property_map_eigen.h"
#include "tbb/tbb.h"

namespace bcg {

void point_cloud_vertex_outlier_probability_knn(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index,
                                                int num_closest, size_t parallel_grain_size) {
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto likelihood = vertices->get_or_add<bcg_scalar_t, 1>("v_likelihood");
    auto outlier_probability = vertices->get_or_add<bcg_scalar_t, 1>("v_outlier_probability");
    auto avg_distance = vertices->get_or_add<bcg_scalar_t, 1>("v_average_distance");
    auto knn = vertices->get_or_add<std::vector<bcg_index_t>, 1>("v_knn");
    auto knn_dists = vertices->get_or_add<VectorS<-1>, 1>("v_knn_distances");
    auto covs = vertices->get_or_add<MatrixS<3, 3>, 1>("v_cov");
    auto normalizers = vertices->get_or_add<bcg_scalar_t, 1>("v_normalizer");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    auto result = index.query_knn(positions[v], num_closest);
                    knn[v] = result.indices;
                    knn_dists[v] = MapConst<bcg_scalar_t>(result.distances);
                    avg_distance[v] = knn_dists[v].mean();

                    covs[v] = MatrixS<3, 3>::Zero(3, 3);

                    for (int j = 0; j < num_closest; ++j) {
                        VectorS<3> diff = positions[v] - positions[knn[v][j]];
                        covs[v] += diff * diff.transpose() *
                                   std::exp(-knn_dists[v][j] * knn_dists[v][j] / (avg_distance[v]));
                    }

                    normalizers[v] = covs[v].trace() * pi / 2.0;
                    covs[v] /= normalizers[v];
                }
            }
    );

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    likelihood[v] = 0;
                    for (int j = 0; j < num_closest; ++j) {
                        VectorS<3> diff = positions[knn[v][j]] - positions[v];
                        likelihood[v] += std::exp(-diff.transpose() * covs[v].inverse() * diff) / num_closest;
                    }
                }
            }
    );

    Map<bcg_scalar_t, 1>(outlier_probability) = 1.0 - MapConst<bcg_scalar_t, 1>(likelihood).array();

    vertices->remove(covs);
    vertices->remove(knn);
    vertices->remove(knn_dists);
    vertices->remove(normalizers);
    vertices->remove(avg_distance);
}

void
point_cloud_vertex_outlier_probability_radius(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index,
                                              bcg_scalar_t radius, size_t parallel_grain_size) {
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto likelihood = vertices->get_or_add<bcg_scalar_t, 1>("v_likelihood");
    auto outlier_probability = vertices->get_or_add<bcg_scalar_t, 1>("v_outlier_probability");
    auto avg_distance = vertices->get_or_add<bcg_scalar_t, 1>("v_average_distance");
    auto knn = vertices->get_or_add<std::vector<bcg_index_t>, 1>("v_knn");
    auto knn_dists = vertices->get_or_add<VectorS<-1>, 1>("v_knn_distances");
    auto covs = vertices->get_or_add<MatrixS<3, 3>, 1>("v_cov");
    auto normalizers = vertices->get_or_add<bcg_scalar_t, 1>("v_normalizer");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    auto result = index.query_radius(positions[v], radius);
                    knn[v] = result.indices;
                    knn_dists[v] = MapConst<bcg_scalar_t>(result.distances);
                    avg_distance[v] = knn_dists[v].mean();

                    covs[v] = MatrixS<3, 3>::Zero(3, 3);

                    for (size_t j = 0; j < knn[v].size(); ++j) {
                        VectorS<3> diff = positions[v] - positions[knn[v][j]];
                        covs[v] += diff * diff.transpose() *
                                   std::exp(-knn_dists[v][j] * knn_dists[v][j] / (avg_distance[v]));
                    }

                    normalizers[v] = covs[v].trace() * pi / 2.0;
                    covs[v] /= normalizers[v];
                }
            }
    );

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    likelihood[v] = 0;
                    for (size_t j = 0; j < knn[v].size(); ++j) {
                        VectorS<3> diff = positions[knn[v][j]] - positions[v];
                        likelihood[v] += std::exp(-diff.transpose() * covs[v].inverse() * diff) / knn[v].size();
                        if (std::isnan(likelihood[v])) likelihood[v] = 0;
                    }
                }
            }
    );

    Map<bcg_scalar_t, 1>(outlier_probability) = 1.0 - MapConst<bcg_scalar_t, 1>(likelihood).array();

    vertices->remove(covs);
    vertices->remove(knn);
    vertices->remove(knn_dists);
    vertices->remove(normalizers);
    vertices->remove(avg_distance);
}

void point_cloud_vertex_remove_outliers(point_cloud &pc, bcg_scalar_t threshold, size_t parallel_grain_size) {
    auto outlier_probability = pc.vertices.get_or_add<bcg_scalar_t, 1>("v_outlier_probability");
    tbb::atomic<size_t> count_deleted = 0;
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) pc.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    if (outlier_probability[v] > threshold) {
                        pc.delete_vertex(v);
                    }
                }
            }
    );

    std::cout << "removed: " << pc.size_vertices_deleted - count_deleted << " vertices.";
    pc.positions.set_dirty();
}

}