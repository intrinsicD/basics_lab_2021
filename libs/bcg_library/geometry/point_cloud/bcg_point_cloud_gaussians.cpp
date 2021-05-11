//
// Created by alex on 06.05.21.
//

#include "bcg_point_cloud_gaussians.h"
#include "math/statistics/bcg_gaussian.h"
#include "tbb/tbb.h"
#include "math/matrix/bcg_matrix_map_eigen.h"

namespace bcg {

void point_cloud_gaussians_knn(point_cloud &pc, const kdtree_property<bcg_scalar_t> &index, int num_closest,
                               bool compute_means, size_t parallel_grain_size) {

    auto means = pc.vertices.get_or_add<VectorS<3>, 3>("v_gaussian_means");
    auto cov = pc.vertices.get_or_add<MatrixS<3, 3>, 9>("v_gaussian_cov");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) pc.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto result = index.query_knn(pc.positions[i], num_closest);
                    std::vector<VectorS<3>> points;
                    means[i].setZero();

                    for (const auto &idx : result.indices) {
                        points.emplace_back(pc.positions[idx]);
                        if (compute_means) {
                            means[i] += pc.positions[idx] / result.indices.size();
                        }
                    }
                    if (!compute_means) {
                        means[i] = pc.positions[i];
                    }
                    Gaussian3 gaussian;
                    gaussian.construct_from_points(MapConst(points), means[i]);
                    cov[i] = gaussian.cov;
                }
            }
    );
    means.set_dirty();
    cov.set_dirty();
}

void point_cloud_gaussians_radius(point_cloud &pc, const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius,
                                  bool compute_means, size_t parallel_grain_size) {
    auto means = pc.vertices.get_or_add<VectorS<3>, 3>("v_gaussian_means");
    auto cov = pc.vertices.get_or_add<MatrixS<3, 3>, 9>("v_gaussian_cov");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) pc.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto result = index.query_radius(pc.positions[i], radius);
                    std::vector<VectorS<3>> points;
                    means[i].setZero();

                    for (const auto &idx : result.indices) {
                        points.emplace_back(pc.positions[idx]);
                        if (compute_means) {
                            means[i] += pc.positions[idx] / result.indices.size();
                        }
                    }
                    if (!compute_means) {
                        means[i] = pc.positions[i];
                    }
                    Gaussian3 gaussian;
                    gaussian.construct_from_points(MapConst(points), means[i]);
                    cov[i] = gaussian.cov;
                }
            }
    );
    means.set_dirty();
    cov.set_dirty();
}

}