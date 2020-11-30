//
// Created by alex on 30.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GRAPH_VERTEX_PCA_H
#define BCG_GRAPHICS_BCG_GRAPH_VERTEX_PCA_H

#include "bcg_graph.h"
#include "math/bcg_pca.h"

namespace bcg {

Pca<3> graph_local_pca_least_squares_svd(halfedge_graph &graph, vertex_handle v, bool compute_mean = true);

Pca<3> graph_local_pca_weighted_least_squares_svd(halfedge_graph &graph, vertex_handle v, bool compute_mean = true);

Pca<3> graph_local_pca_least_squares_eig(halfedge_graph &graph, vertex_handle v, bool compute_mean = true);

Pca<3> graph_local_pca_weighted_least_squares_eig(halfedge_graph &graph, vertex_handle v, bool compute_mean = true);

void graph_local_pcas(halfedge_graph &graph, std::function<Pca<3>(halfedge_graph &, vertex_handle, bool)> method,
                      bool compute_mean = true, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_GRAPH_VERTEX_PCA_H
