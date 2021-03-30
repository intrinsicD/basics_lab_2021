//
// Created by alex on 30.11.20.
//

#include "bcg_point_cloud_vertex_pca.h"
#include "bcg_property_map_eigen.h"
#include "tbb/tbb.h"

namespace bcg {

Pca<3> point_cloud_vertex_pca_least_squares_svd(const MatrixS<-1, 3> &P, const VectorS<3> &point, bool compute_mean) {
    Pca<3> pca;
    VectorS<3> mean = point;
    if (compute_mean) {
        mean = P.colwise().mean();
    }
    least_squares_fit_svd<-1, 3>(pca, P, mean);
    return pca;
}

Pca<3>
point_cloud_vertex_pca_weighted_least_squares_svd(const MatrixS<-1, 3> &P, const VectorS<3> &point, bool compute_mean) {
    Pca<3> pca;
    VectorS<3> mean = point;
    if (compute_mean) {
        mean = P.colwise().mean();
    }
    VectorS<-1> weights((P.rowwise() - mean.transpose()).rowwise().norm());
    weighted_least_squares_fit_svd<-1, 3>(pca, P, mean, weights);
    return pca;
}

Pca<3> point_cloud_vertex_pca_least_squares_eig(const MatrixS<-1, 3> &P, const VectorS<3> &point, bool compute_mean) {
    Pca<3> pca;
    VectorS<3> mean = point;
    if (compute_mean) {
        mean = P.colwise().mean();
    }
    least_squares_fit_eig<-1, 3>(pca, P, mean);
    return pca;
}

Pca<3> point_cloud_vertex_pca_weighted_least_squares_eig(const MatrixS<-1, 3> &P, const VectorS<3> &point, bool compute_mean) {
    Pca<3> pca;
    VectorS<3> mean = point;
    if (compute_mean) {
        mean = P.colwise().mean();
    }
    VectorS<-1> weights((P.rowwise() - mean.transpose()).rowwise().norm());
    weighted_least_squares_fit_eig<-1, 3>(pca, P, mean, weights);
    return pca;
}

void point_cloud_vertex_pcas_knn(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index, int num_closest,
                             std::function<Pca<3>(const MatrixS<-1, 3> &, const VectorS<3> &, bool)> method,
                             bool compute_mean, size_t parallel_grain_size) {
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto v_pca_mean = vertices->get_or_add<VectorS<3>, 3>("v_pca_mean");
    auto v_pca_normal = vertices->get_or_add<VectorS<3>, 3>("v_pca_normal");
    auto v_pca_tangent1 = vertices->get_or_add<VectorS<3>, 3>("v_pca_tangent1");
    auto v_pca_tangent2 = vertices->get_or_add<VectorS<3>, 3>("v_pca_tangent2");
    auto v_pca_normal_loading = vertices->get_or_add<bcg_scalar_t, 1>("v_pca_normal_loading");
    auto v_pca_tangent1_loading = vertices->get_or_add<bcg_scalar_t, 1>("v_pca_tangent1_loading");
    auto v_pca_tangent2_loading = vertices->get_or_add<bcg_scalar_t, 1>("v_pca_tangent2_loading");
    auto v_pca_splat_size = vertices->get_or_add<bcg_scalar_t, 1>("v_pca_splat_size");
    auto v_pca_loading = vertices->get_or_add<VectorS<3>, 3>("v_pca_loading");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                MatrixS<-1, 3> P;
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    auto result = index.query_knn(positions[v], num_closest);
                    P.resize(result.indices.size(), 3);
                    for(size_t i = 0; i < result.indices.size(); ++i){
                        P.row(i) = positions[result.indices[i]];
                    }
                    auto pca = method(P, positions[v], compute_mean);

                    v_pca_tangent1[v] = pca.directions.col(0);
                    v_pca_tangent2[v] = pca.directions.col(1);
                    v_pca_normal[v] = pca.directions.col(2);
                    v_pca_tangent1_loading[v] = pca.loadings(0);
                    v_pca_tangent2_loading[v] = pca.loadings(1);
                    v_pca_splat_size[v] = std::exp(-pca.loadings(2)) * (v_pca_tangent1_loading[v] + v_pca_tangent2_loading[v]) / 2.0;
                    v_pca_normal_loading[v] = pca.loadings(2);
                    v_pca_loading[v] = pca.loadings;
                    v_pca_mean[v] = pca.mean;
                }
            }
    );
    v_pca_mean.set_dirty();
    v_pca_normal.set_dirty();
    v_pca_tangent1.set_dirty();
    v_pca_tangent2.set_dirty();
    v_pca_normal_loading.set_dirty();
    v_pca_tangent1_loading.set_dirty();
    v_pca_tangent2_loading.set_dirty();
    v_pca_loading.set_dirty();
    v_pca_splat_size.set_dirty();
    v_pca_mean.set_dirty();
}


void
point_cloud_vertex_pcas_radius(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius,
                        std::function<Pca<3>(const MatrixS<-1, 3> &, const VectorS<3> &, bool)> method,
                        bool compute_mean, size_t parallel_grain_size) {
    auto positions = vertices->get<VectorS<3>, 3>("v_position");
    auto v_pca_mean = vertices->get_or_add<VectorS<3>, 3>("v_pca_mean");
    auto v_pca_normal = vertices->get_or_add<VectorS<3>, 3>("v_pca_normal");
    auto v_pca_tangent1 = vertices->get_or_add<VectorS<3>, 3>("v_pca_tangent1");
    auto v_pca_tangent2 = vertices->get_or_add<VectorS<3>, 3>("v_pca_tangent2");
    auto v_pca_normal_loading = vertices->get_or_add<bcg_scalar_t, 1>("v_pca_normal_loading");
    auto v_pca_tangent1_loading = vertices->get_or_add<bcg_scalar_t, 1>("v_pca_tangent1_loading");
    auto v_pca_tangent2_loading = vertices->get_or_add<bcg_scalar_t, 1>("v_pca_tangent2_loading");
    auto v_pca_loading = vertices->get_or_add<VectorS<3>, 3>("v_pca_loading");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) vertices->size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                MatrixS<-1, 3> P;
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    auto result = index.query_radius(positions[v], radius);
                    P.resize(result.indices.size(), 3);
                    for(size_t i = 0; i < result.indices.size(); ++i){
                        P.row(i) = positions[result.indices[i]];
                    }
                    auto pca = method(P, positions[v], compute_mean);

                    v_pca_tangent1[v] = pca.directions.col(0);
                    v_pca_tangent2[v] = pca.directions.col(1);
                    v_pca_normal[v] = pca.directions.col(2);
                    v_pca_tangent1_loading[v] = pca.loadings(0);
                    v_pca_tangent2_loading[v] = pca.loadings(1);
                    v_pca_normal_loading[v] = pca.loadings(2);
                    v_pca_loading[v] = pca.loadings;
                    v_pca_mean[v] = pca.mean;
                }
            }
    );
    v_pca_mean.set_dirty();
    v_pca_normal.set_dirty();
    v_pca_tangent1.set_dirty();
    v_pca_tangent2.set_dirty();
    v_pca_normal_loading.set_dirty();
    v_pca_tangent1_loading.set_dirty();
    v_pca_tangent2_loading.set_dirty();
    v_pca_loading.set_dirty();
    v_pca_mean.set_dirty();
}

}