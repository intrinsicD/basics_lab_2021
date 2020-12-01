//
// Created by alex on 30.11.20.
//

#ifndef BCG_GRAPHICS_BCG_POINT_CLOUD_VERTEX_PCA_H
#define BCG_GRAPHICS_BCG_POINT_CLOUD_VERTEX_PCA_H

#include "graph/bcg_graph.h"
#include "kdtree/bcg_kdtree.h"
#include "math/bcg_pca.h"

namespace bcg {

Pca<3> point_cloud_vertex_pca_least_squares_svd(const MatrixS<-1, 3> &P, const VectorS<3> &point, bool compute_mean);

Pca<3>
point_cloud_vertex_pca_weighted_least_squares_svd(const MatrixS<-1, 3> &P, const VectorS<3> &point, bool compute_mean);

Pca<3> point_cloud_vertex_pca_least_squares_eig(const MatrixS<-1, 3> &P, const VectorS<3> &point, bool compute_mean);

Pca<3> point_cloud_vertex_pca_weighted_least_squares_eig(const MatrixS<-1, 3> &P, const VectorS<3> &point, bool compute_mean);

void point_cloud_vertex_pcas_knn(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index, int num_closest,
                             std::function<Pca<3>(const MatrixS<-1, 3> &, const VectorS<3> &, bool)> method,
                             bool compute_mean = true,
                             size_t parallel_grain_size = 1024);

void
point_cloud_vertex_pcas_radius(vertex_container *vertices, const kdtree_property<bcg_scalar_t> &index, bcg_scalar_t radius,
                        std::function<Pca<3>(const MatrixS<-1, 3> &, const VectorS<3> &, bool)> method,
                        bool compute_mean = true, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_POINT_CLOUD_VERTEX_PCA_H
