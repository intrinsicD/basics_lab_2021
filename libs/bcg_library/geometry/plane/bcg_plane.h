//
// Created by alex on 02.12.20.
//

#ifndef BCG_GRAPHICS_BCG_PLANE_H
#define BCG_GRAPHICS_BCG_PLANE_H

#include "math/vector/bcg_vector.h"
#include "math/matrix/bcg_matrix.h"
#include "math/bcg_pca.h"

namespace bcg {

template<int N>
struct Plane {
    VectorS<N> normal;
    bcg_scalar_t distance;

    plane() : normal(zeros<N>), distance(0) {}

    plane(const VectorS<N> &normal, bcg_scalar_t distance) : normal(normal), distance(distance) {}

    plane(const VectorS<N> &normal, const VectorS<N> &point) : normal(normal), distance(normal.dot(point)) {}

    VectorS<N> basepoint() const {
        return normal * distance;
    }
};

template<int N>
std::ostream &operator<<(std::ostream &stream, const Plane<N> &plane) {
    stream << "normal: " << plane.normal << std::endl;
    stream << "distance: " << plane.distance << std::endl;
    return stream;
}

using plane2 = Plane<2>;

using plane3 = Plane<3>;

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
    for(size_t i = 0; i < max_iters; ++i){
        weighted_least_squares_fit_svd(current_plane, points, mean, W);
        W = (-(points * current_plane.normal).array().squared()).exp();
        bcg_scalar_t current_sum = W.sum();
        if(current_sum > best_weight_sum){
            best_weight_sum = current_sum;
        }else{
            break;
        }
    }
    plane = current_plane;
}

}

#endif //BCG_GRAPHICS_BCG_PLANE_H
