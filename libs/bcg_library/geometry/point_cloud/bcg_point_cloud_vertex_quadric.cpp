//
// Created by alex on 03.12.20.
//

#include "bcg_point_cloud_vertex_quadric.h"
#include "math/bcg_quadric_fit.h"
#include "tbb/tbb.h"

namespace bcg {

std::vector<std::string> quadric_names() {
    std::vector<std::string> names(static_cast<int>(PointCloudQuadricTypes::__last__));
    names[static_cast<int>(PointCloudQuadricTypes::point)] = "point";
    names[static_cast<int>(PointCloudQuadricTypes::plane)] = "plane";
    names[static_cast<int>(PointCloudQuadricTypes::isotropic)] = "isotropic";
    names[static_cast<int>(PointCloudQuadricTypes::anisotropic)] = "anisotropic";
    names[static_cast<int>(PointCloudQuadricTypes::fit)] = "fit";
    return names;
}


quadric point_quadric(vertex_handle v, property<VectorS<3>, 3> positions, const neighbors_query &,
                      property<VectorS<3>, 3>) {
    return quadric::point_quadric(positions[v]);
}

quadric plane_quadric(vertex_handle v, property<VectorS<3>, 3> positions, const neighbors_query &,
                      property<VectorS<3>, 3> normals) {
    return quadric::plane_quadric(positions[v], normals[v]);
}

quadric
probabilistic_plane_quadric_isotropic(vertex_handle v, property<VectorS<3>, 3> positions, const neighbors_query &,
                                      property<VectorS<3>, 3> normals) {
    return quadric::probabilistic_plane_quadric(positions[v], normals[v], 0, 1);
}

quadric probabilistic_plane_quadric_anisotropic(vertex_handle v, property<VectorS<3>, 3> positions,
                                                const neighbors_query &result, property<VectorS<3>, 3> normals) {
    MatrixS<3, 3> cov_p(MatrixS<3, 3>::Zero());
    MatrixS<3, 3> cov_n(MatrixS<3, 3>::Zero());
    for (const auto &idx : result.indices) {
        VectorS<3> dp = positions[idx] - positions[v];
        cov_p += dp * dp.transpose();
        VectorS<3> dn = normals[idx] - normals[v];
        cov_n += dn * dn.transpose();
    }
    cov_p /= (result.indices.size() - 1);
    cov_n /= (result.indices.size() - 1);
    return quadric::probabilistic_plane_quadric(positions[v], normals[v], cov_p, cov_n);
}

quadric fit(vertex_handle v, property<VectorS<3>, 3> positions,
            const neighbors_query &result,
            property<VectorS<3>, 3>) {
    MatrixS<-1, 3> P(result.indices.size(), 3);
    for (long i = 0; i < static_cast<long>(result.indices.size()); ++i) {
        P.row(i) = positions[result.indices[i]];
    }
    quadric Q;
    bcg_scalar_t error = fit(P, Q);
    return Q;
}

void point_cloud_vertex_quadric_knn(vertex_container *vertices,
                                    std::function<quadric(vertex_handle, property<VectorS<3>, 3>,
                                                          const neighbors_query &, property<VectorS<3>, 3>)> method,
                                    const kdtree_property<bcg_scalar_t> &index, int num_closest,
                                    size_t parallel_grain_size) {
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto normals = vertices->get<VectorS<3>, 3>("v_normal");
    auto quadrics = vertices->get_or_add<quadric, 1>("v_quadric");
    auto coeff_c = vertices->get_or_add<bcg_scalar_t, 1>("v_quadric_c");
    auto quadric_normals = vertices->get_or_add<VectorS<3>, 3>("v_quadric_normal");
    auto neighbors = vertices->get_or_add<neighbors_query, 1>("v_neighbors");
    auto quadrics_minimizer = vertices->get_or_add<VectorS<3>, 3>("v_quadric_minimizer");
    if (!normals) {
        normals = vertices->get<VectorS<3>, 3>("v_pca_normal");
    }
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    auto result = index.query_knn(positions[v], num_closest);
                    neighbors[v] = result;
                    quadrics[v] = method(v, positions, result, normals);
                    coeff_c[v] = quadrics[v].c;
                    quadric_normals[v] = quadrics[v].normal(positions[v]);
                    quadrics_minimizer[v] = quadric_minimizer(quadrics[v]);
                }
            }
    );
    coeff_c.set_dirty();
    quadric_normals.set_dirty();
    quadrics_minimizer.set_dirty();
}

void point_cloud_vertex_quadric_radius(vertex_container *vertices,
                                       std::function<quadric(vertex_handle, property<VectorS<3>, 3>,
                                                             const neighbors_query &, property<VectorS<3>, 3>)> method,
                                       const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius,
                                       size_t parallel_grain_size) {
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto normals = vertices->get<VectorS<3>, 3>("v_normal");
    auto quadrics = vertices->get_or_add<quadric, 1>("v_quadric");
    auto coeff_c = vertices->get_or_add<bcg_scalar_t, 1>("v_quadric_c");
    auto neighbors = vertices->get_or_add<neighbors_query, 1>("v_neighbors");
    auto quadric_normals = vertices->get_or_add<VectorS<3>, 3>("v_quadric_normal");
    auto quadrics_minimizer = vertices->get_or_add<VectorS<3>, 3>("v_quadric_minimizer");
    if (!normals) {
        normals = vertices->get<VectorS<3>, 3>("v_pca_normal");
    }

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    auto result = index.query_radius(positions[v], radius);
                    neighbors[v] = result;
                    quadrics[v] = method(v, positions, result, normals);
                    coeff_c[v] = quadrics[v].c;
                    quadric_normals[v] = quadrics[v].normal(positions[v]);
                    quadrics_minimizer[v] = quadric_minimizer(quadrics[v]);
                }
            }
    );
    coeff_c.set_dirty();
    quadric_normals.set_dirty();
    quadrics_minimizer.set_dirty();
}

void point_cloud_vertex_quadric_sum_neighbors(vertex_container *vertices, size_t parallel_grain_size){
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto quadrics_error = vertices->get_or_add<bcg_scalar_t, 1>("v_quadric_error");
    auto quadrics_minimizer = vertices->get_or_add<VectorS<3>, 3>("v_quadric_minimizer");
    auto quadrics = vertices->get<quadric, 1>("v_quadric");
    auto quadric_sums = vertices->get_or_add<quadric, 1>("v_quadric_sum");
    auto neighbors = vertices->get<neighbors_query, 1>("v_neighbors");
    auto coeff_c = vertices->get_or_add<bcg_scalar_t, 1>("v_quadric_c");
    auto quadric_normals = vertices->get_or_add<VectorS<3>, 3>("v_quadric_normal");
    if (!neighbors || !quadrics) {
        std::cout << "Please compute the quadrics and neighborhoods first!\n";
        return;
    }
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    quadric Q;
                    for (const auto &idx : neighbors[v].indices) {
                        Q += quadrics[idx];
                    }

                    quadric_sums[v] = Q;
                    quadrics_error[v] = quadric_sums[v](positions[v]);
                    quadrics_minimizer[v] = quadric_minimizer(quadric_sums[v]);
                    coeff_c[v] = quadric_sums[v].c;
                    quadric_normals[v] = quadric_sums[v].normal(positions[v]);
                }
            }
    );
    coeff_c.set_dirty();
    quadrics_error.set_dirty();
    quadrics_minimizer.set_dirty();
    quadric_normals.set_dirty();
    quadrics.vector() = quadric_sums.vector();
}

void point_cloud_vertex_quadric_avg_neighbors(vertex_container *vertices, size_t parallel_grain_size){
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto quadrics_error = vertices->get_or_add<bcg_scalar_t, 1>("v_quadric_error");
    auto quadrics_minimizer = vertices->get_or_add<VectorS<3>, 3>("v_quadric_minimizer");
    auto quadrics = vertices->get<quadric, 1>("v_quadric");
    auto quadric_avg = vertices->get_or_add<quadric, 1>("v_quadric_avg");
    auto neighbors = vertices->get<neighbors_query, 1>("v_neighbors");
    auto coeff_c = vertices->get_or_add<bcg_scalar_t, 1>("v_quadric_c");
    auto quadric_normals = vertices->get_or_add<VectorS<3>, 3>("v_quadric_normal");
    if (!neighbors || !quadrics) {
        std::cout << "Please compute the quadrics and neighborhoods first!\n";
        return;
    }
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    quadric Q;
                    for (const auto &idx : neighbors[v].indices) {
                        Q += quadrics[idx];
                    }

                    quadric_avg[v] = Q / bcg_scalar_t(neighbors[v].indices.size());
                    quadrics_error[v] = quadric_avg[v](positions[v]);
                    quadrics_minimizer[v] = quadric_minimizer(quadric_avg[v]);
                    coeff_c[v] = quadric_avg[v].c;
                    quadric_normals[v] = quadric_avg[v].normal(positions[v]);
                }
            }
    );
    coeff_c.set_dirty();
    quadrics_error.set_dirty();
    quadrics_minimizer.set_dirty();
    quadric_normals.set_dirty();
    quadrics.vector() = quadric_avg.vector();
}

}