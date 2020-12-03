//
// Created by alex on 03.12.20.
//

#include "bcg_point_cloud_vertex_quadric.h"
#include "tbb/tbb.h"

namespace bcg {

std::vector<std::string> quadric_names(){
    std::vector<std::string> names(static_cast<int>(PointCloudQuadricTypes::__last__));
    names[static_cast<int>(PointCloudQuadricTypes::point)] = "point";
    names[static_cast<int>(PointCloudQuadricTypes::plane)] = "plane";
    names[static_cast<int>(PointCloudQuadricTypes::isotropic)] = "isotropic";
    names[static_cast<int>(PointCloudQuadricTypes::anisotropic)] = "anisotropic";
    return names;
}


quadric point_quadric(vertex_handle v, property<VectorS<3>, 3> positions, const neighbors_query &result,
                      property<VectorS<3>, 3> normals) {
    quadric Q;
    Q.point_quadric(positions[v]);
    return Q;
}

quadric plane_quadric(vertex_handle v, property<VectorS<3>, 3> positions, const neighbors_query &result,
                      property<VectorS<3>, 3> normals) {
    quadric Q;
    Q.plane_quadric(positions[v], normals[v]);
    return Q;
}

quadric
probabilistic_plane_quadric_isotropic(vertex_handle v, property<VectorS<3>, 3> positions, const neighbors_query &result,
                                      property<VectorS<3>, 3> normals) {
    quadric Q;
    Q.probabilistic_plane_quadric(positions[v], normals[v], 0, 1);
    return Q;
}

quadric probabilistic_plane_quadric_anisotropic(vertex_handle v, property<VectorS<3>, 3> positions,
                                                const neighbors_query &result, property<VectorS<3>, 3> normals) {
    MatrixS<3, 3> cov_p(MatrixS<3, 3>::Zero());
    MatrixS<3, 3> cov_n(MatrixS<3, 3>::Zero());
    for (const auto &idx : result.indices) {
        VectorS<3> dp = positions[idx] - positions[v];
        cov_p += dp * dp.transpose();
        VectorS<3> dn = normals[idx];
        cov_n += dn * dn.transpose();
    }
    cov_p /= (result.indices.size() - 1);
    cov_n /= (result.indices.size() - 1);

    quadric Q;
    Q.probabilistic_plane_quadric(positions[v], normals[v], cov_p, cov_n);
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
    auto neighbors = vertices->get_or_add<neighbors_query, 1>("v_neighbors");
    if(!normals){
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
                }
            }
    );
}

void point_cloud_vertex_quadric_radius(vertex_container *vertices,
                                       std::function<quadric(vertex_handle, property<VectorS<3>, 3>,
                                                             const neighbors_query &, property<VectorS<3>, 3>)> method,
                                       const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius,
                                       size_t parallel_grain_size) {
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto normals = vertices->get<VectorS<3>, 3>("v_normal");
    auto quadrics = vertices->get_or_add<quadric, 1>("v_quadric");
    auto neighbors = vertices->get_or_add<neighbors_query, 1>("v_neighbors");
    if(!normals){
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
                }
            }
    );
}

void point_cloud_vertex_quadric_collect_neighbors(vertex_container *vertices, size_t parallel_grain_size){
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto quadrics_error = vertices->get_or_add<bcg_scalar_t , 1>("v_quadric_error");
    auto quadrics_minimizer = vertices->get_or_add<VectorS<3> , 3>("v_quadric_minimizer");
    auto quadrics = vertices->get<quadric, 1>("v_quadric");
    auto quadric_sums = vertices->get_or_add<quadric, 1>("v_quadric_sum");
    auto neighbors = vertices->get<neighbors_query, 1>("v_neighbors");

    if(!neighbors || !quadrics) {
        std::cout << "Please compute the quadrics and neighborhoods first!\n";
        return;
    }
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    quadric Q;
                    for(const auto &idx : neighbors[v].indices){
                        Q += quadrics[v];
                    }

                    quadric_sums[v] = Q;
                    quadrics_error[v] = Q(positions[v]);
                    quadrics_minimizer[v] = Q.minimizer();
                }
            }
    );
    vertices->remove(neighbors);
}

}