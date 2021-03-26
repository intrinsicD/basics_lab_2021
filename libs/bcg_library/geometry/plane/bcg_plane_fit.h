//
// Created by alex on 25.03.21.
//

#ifndef BCG_GRAPHICS_BCG_PLANE_FIT_H
#define BCG_GRAPHICS_BCG_PLANE_FIT_H

#include "bcg_plane.h"

namespace bcg{

template<int N>
void least_squares_fit_svd(Plane<N> &plane, const MatrixS<-1, N> &points, const VectorS<N> &mean) {
    if (points.rows() < 3) {
        plane = Plane < N > (VectorS<N>::Unit(N - 1), mean);
        return;
    }

    Pca<N> pca;
    least_squares_fit_svd(pca, points, mean);
    plane = plane<N>(pca.directions.col(N - 1), mean);
}

template<int N>
void weighted_least_squares_fit_svd(Plane<N> &plane, const MatrixS<-1, N> &points, const VectorS<N> &mean, const VectorS<N> &weights) {
    if (points.rows() < 3) {
        plane = Plane < N > (VectorS<N>::Unit(N - 1), mean);
        return;
    }

    Pca<N> pca;
    weighted_least_squares_fit_svd(pca, points, mean, weights);
    plane = plane<N>(pca.directions.col(N - 1), mean);
}

template<int N>
void robust_least_squares_fit_svd(Plane<N> &plane, const MatrixS<-1, N> &points, const VectorS<N> &mean, const VectorS<N> &weights, int max_iters = 1) {
    if (points.rows() < 3) {
        plane = Plane < N > (VectorS<N>::Unit(N - 1), mean);
        return;
    }

    VectorS<N> W = weights;
    bcg_scalar_t best_weight_sum = W.sum();

    Plane<N> current_plane;
    for(bcg_size_t i = 0; i < max_iters; ++i){
        weighted_least_squares_fit_svd(current_plane, points, mean, W);
        bcg_scalar_t current_sum = 0;
        for(bcg_size_t j = 0; j < points.rows(); ++j){
            W[j] = std::exp(-plane.distance_squared(points.row(j)));
            current_sum += W[j];
        }

        if(current_sum > best_weight_sum){
            best_weight_sum = current_sum;
        }else{
            break;
        }
    }
    plane = current_plane;
}

}

#endif //BCG_GRAPHICS_BCG_PLANE_FIT_H
