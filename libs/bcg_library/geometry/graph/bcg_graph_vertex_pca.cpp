//
// Created by alex on 30.11.20.
//

#include "bcg_graph_vertex_pca.h"
#include "math/matrix/bcg_matrix_map_eigen.h"
#include "tbb/tbb.h"

namespace bcg {

Pca<3> graph_vertex_pca_least_squares_svd(halfedge_graph &graph, vertex_handle v, bool compute_mean) {
    std::vector<VectorS<3>> points;
    for (const auto vv : graph.get_vertices(v)) {
        points.push_back(graph.positions[vv]);
    }
    Pca<3> pca;
    MatrixS<-1, 3> P(MapConst<3>(points));
    VectorS<3> mean = graph.positions[v];
    if (compute_mean) {
        mean = P.colwise().mean();
    }
    least_squares_fit_svd<-1, 3>(pca, P, mean);
    return pca;
}

Pca<3> graph_vertex_pca_weighted_least_squares_svd(halfedge_graph &graph, vertex_handle v, bool compute_mean) {
    std::vector<VectorS<3>> points;
    for (const auto h : graph.get_halfedges(v)) {
        auto vv = graph.get_to_vertex(h);
        points.push_back(graph.positions[vv]);
    }
    Pca<3> pca;
    MatrixS<-1, 3> P(MapConst<3>(points));
    VectorS<3> mean = graph.positions[v];
    if (compute_mean) {
        mean = P.colwise().mean();
    }
    VectorS<-1> weights((P.rowwise() - mean.transpose()).rowwise().norm());
    weighted_least_squares_fit_svd<-1, 3>(pca, P, mean, weights);
    return pca;
}

Pca<3> graph_vertex_pca_least_squares_eig(halfedge_graph &graph, vertex_handle v, bool compute_mean) {
    std::vector<VectorS<3>> points;
    for (const auto h : graph.get_halfedges(v)) {
        auto vv = graph.get_to_vertex(h);
        points.push_back(graph.positions[vv]);
    }
    Pca<3> pca;
    MatrixS<-1, 3> P(MapConst<3>(points));
    VectorS<3> mean = graph.positions[v];
    if (compute_mean) {
        mean = P.colwise().mean();
    }
    least_squares_fit_eig<-1, 3>(pca, P, mean);
    return pca;
}

Pca<3> graph_vertex_pca_weighted_least_squares_eig(halfedge_graph &graph, vertex_handle v, bool compute_mean) {
    std::vector<VectorS<3>> points;
    for (const auto h : graph.get_halfedges(v)) {
        auto vv = graph.get_to_vertex(h);
        points.push_back(graph.positions[vv]);
    }
    Pca<3> pca;
    MatrixS<-1, 3> P(MapConst<3>(points));
    VectorS<3> mean = graph.positions[v];
    if (compute_mean) {
        mean = P.colwise().mean();
    }
    VectorS<-1> weights((P.rowwise() - mean.transpose()).rowwise().norm());
    weighted_least_squares_fit_eig<-1, 3>(pca, P, mean, weights);
    return pca;
}

void graph_vertex_pcas(halfedge_graph &graph, std::function<Pca<3>(halfedge_graph &, vertex_handle, bool)> method,
                       bool compute_mean, size_t parallel_grain_size) {
    auto v_pca_mean = graph.vertices.get_or_add<VectorS<3>, 3>("v_pca_mean");
    auto v_pca_normal = graph.vertices.get_or_add<VectorS<3>, 3>("v_pca_normal");
    auto v_pca_tangent1 = graph.vertices.get_or_add<VectorS<3>, 3>("v_pca_tangent1");
    auto v_pca_tangent2 = graph.vertices.get_or_add<VectorS<3>, 3>("v_pca_tangent2");
    auto v_pca_normal_loading = graph.vertices.get_or_add<bcg_scalar_t, 1>("v_pca_normal_loading");
    auto v_pca_tangent1_loading = graph.vertices.get_or_add<bcg_scalar_t, 1>("v_pca_tangent1_loading");
    auto v_pca_tangent2_loading = graph.vertices.get_or_add<bcg_scalar_t, 1>("v_pca_tangent2_loading");
    auto v_pca_loading = graph.vertices.get_or_add<VectorS<3>, 3>("v_pca_loading");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) graph.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    auto pca = method(graph, v, compute_mean);

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