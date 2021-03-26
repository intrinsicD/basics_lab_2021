//
// Created by alex on 25.03.21.
//

#ifndef BCG_GRAPHICS_BCG_PARAMETERIZED_PLANE_FIT_H
#define BCG_GRAPHICS_BCG_PARAMETERIZED_PLANE_FIT_H

#include "bcg_parameterized_plane.h"
#include "math/bcg_pca.h"

namespace bcg {

enum class ReparameterizedFittingTypes{
    least_squared_svd,
    weighted_least_squared_svd,
    robust_weighted_least_squared_svd,
    __last__
};

inline std::vector<std::string> parameterized_plane_fit_names() {
    std::vector<std::string> names(static_cast<int>(ReparameterizedFittingTypes::__last__));
    names[static_cast<int>(ReparameterizedFittingTypes::least_squared_svd)] = "least_squared_svd";
    names[static_cast<int>(ReparameterizedFittingTypes::weighted_least_squared_svd)] = "weighted_least_squared_svd";
    names[static_cast<int>(ReparameterizedFittingTypes::robust_weighted_least_squared_svd)] = "robust_weighted_least_squared_svd";
    return names;
}

template<int N>
void least_squares_fit_svd(ParameterizedPlane<N> &plane, const MatrixS<-1, N> &points, const VectorS<N> &mean) {
    if (points.rows() < 3) {
        plane = ParameterizedPlane<N>(mean);
        return;
    }

    Pca<N> pca;
    least_squares_fit_svd(pca, points, mean);
    plane = ParameterizedPlane<N>(mean, pca.directions.block(0, 0, N, 2));
    plane.directions.col(0) *= pca.loadings[0] * 2;
    plane.directions.col(1) *= pca.loadings[1] * 2;
}

template<int N>
void weighted_least_squares_fit_svd(ParameterizedPlane<N> &plane, const MatrixS<-1, N> &points, const VectorS<N> &mean,
                                    const VectorS<-1> &weights) {
    if (points.rows() < 3) {
        plane = ParameterizedPlane<N>(mean);
        return;
    }

    Pca<N> pca;
    weighted_least_squares_fit_svd(pca, points, mean, weights);
    plane = ParameterizedPlane<N>(mean, pca.directions.block(0, 0, N, 2));
    plane.directions.col(0) *= pca.loadings[0] * 2;
    plane.directions.col(1) *= pca.loadings[1] * 2;
}

template<int N>
void robust_least_squares_fit_svd(ParameterizedPlane<N> &plane, const MatrixS<-1, N> &points, const VectorS<N> &mean, const VectorS<-1> &weights, int max_iters = 1) {
    if (points.rows() < 3) {
        plane = ParameterizedPlane<N>(mean);
        return;
    }

    VectorS<-1> W = weights;
    bcg_scalar_t best_weight_sum = W.sum();
    if(best_weight_sum == 0){
        W.setOnes();
    }

    ParameterizedPlane<N> current_plane;
    for(bcg_size_t i = 0; i < max_iters; ++i){
        weighted_least_squares_fit_svd(current_plane, points, mean, W);
        bcg_scalar_t current_sum = 0;
        for(bcg_size_t j = 0; j < points.rows(); ++j){
            W[j] = std::exp(-current_plane.distance_squared(points.row(j)));
            current_sum += W[j];
        }

        W /= current_sum;

        if(current_sum > best_weight_sum){
            best_weight_sum = current_sum;
        }else{
            break;
        }
    }
    plane = current_plane;
}


}

#endif //BCG_GRAPHICS_BCG_PARAMETERIZED_PLANE_FIT_H
