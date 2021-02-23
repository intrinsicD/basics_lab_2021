//
// Created by alex on 17.02.21.
//

#ifndef BCG_GRAPHICS_BCG_VECTOR_MEDIAN_FILTER_DIRECTIONAL_H
#define BCG_GRAPHICS_BCG_VECTOR_MEDIAN_FILTER_DIRECTIONAL_H

#include "bcg_vector.h"
#include "math/matrix/bcg_matrix_map_eigen.h"
#include "math/matrix/bcg_matrix_pairwise_distances.h"

namespace bcg{

template<typename Derived>
Vector<typename Derived::Scalar, -1> vector_median_filter_directional(const Eigen::EigenBase<Derived> &V){
    Matrix<typename Derived::Scalar, -1, -1> VV = V.derived().rowwise().normalized();
    Matrix<typename Derived::Scalar, -1, -1> PA = (1.0 - pairwise_distance_squared(VV, VV).array() / 2.0).acos().matrix();
    Vector<typename Derived::Scalar, -1> sum = PA.rowwise().sum();
    long median_idx;
    sum.minCoeff(&median_idx);
    return V.derived().row(median_idx);
}

template<typename T, int D>
Vector<T, D> vector_median_filter_directional(const std::vector<Vector<T, D>> &V){
    return vector_median_filter_directional(MapConst(V));
}

}

#endif //BCG_GRAPHICS_BCG_VECTOR_MEDIAN_FILTER_DIRECTIONAL_H
