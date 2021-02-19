//
// Created by alex on 17.02.21.
//

#ifndef BCG_GRAPHICS_BCG_VECTOR_MEDIAN_FILTER_FUZZY_H
#define BCG_GRAPHICS_BCG_VECTOR_MEDIAN_FILTER_FUZZY_H

#include "bcg_vector.h"
#include "math/matrix/bcg_matrix_map_eigen.h"
#include "math/matrix/bcg_matrix_pairwise_distances.h"

namespace bcg{

template<typename T, int D>
void vector_median_filter_fuzzy(const std::vector<Vector<T, D>> &V, bcg_scalar_t sigma){
    return vector_median_filter_fuzzy(MapConst(V));
}

template<typename Derived>
Vector<typename Derived::Scalar, -1> vector_median_filter_fuzzy(const Eigen::EigenBase<Derived> &V, bcg_scalar_t sigma){
    Derived PD = pairwise_distance(V, V);
    Vector<typename Derived::Scalar, -1> sum = PD.rowwise().sum();
    auto median_idx = sum.minCoeff();
    Vector<typename Derived::Scalar, -1> mu = (-pairwise_distance_squared(V.row(median_idx), V) / (2.0 * sigma * sigma)).exp();
    return mu.transpose() * V / mu.sum();
}

template<typename T, int D>
void vector_median_filter_directional_fuzzy(const std::vector<Vector<T, D>> &V, bcg_scalar_t sigma){
    return vector_median_filter_directional_fuzzy(MapConst(V));
}

template<typename Derived>
Vector<typename Derived::Scalar, -1> vector_median_filter_directional_fuzzy(const Eigen::EigenBase<Derived> &V, bcg_scalar_t sigma){
    Derived PA = (1.0 - pairwise_distance_squared(V.rowwise().normalized(), V.rowwise().normalized()).array() / 2.0).acos();
    Vector<typename Derived::Scalar, -1> sum = PA.rowwise().sum();
    auto median_idx = sum.minCoeff();
    Vector<typename Derived::Scalar, -1> mu = (-pairwise_distance_squared(V.row(median_idx), V) / (2.0 * sigma * sigma)).exp();
    return mu.transpose() * V / mu.sum();
}

}

#endif //BCG_GRAPHICS_BCG_VECTOR_MEDIAN_FILTER_FUZZY_H
