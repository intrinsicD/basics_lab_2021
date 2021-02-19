//
// Created by alex on 17.02.21.
//

#ifndef BCG_GRAPHICS_BCG_VECTOR_MEDIAN_FILTER_H
#define BCG_GRAPHICS_BCG_VECTOR_MEDIAN_FILTER_H

#include "bcg_vector.h"
#include "math/matrix/bcg_matrix_map_eigen.h"
#include "math/matrix/bcg_matrix_pairwise_distances.h"

namespace bcg{

template<typename T, int D>
void vector_median_filter(const std::vector<Vector<T, D>> &V){
    return vector_median_filter(MapConst(V));
}

template<typename Derived>
Vector<typename Derived::Scalar, -1> vector_median_filter(const Eigen::EigenBase<Derived> &V){
    Derived PD = pairwise_distance(V, V);
    Vector<typename Derived::Scalar, -1> sum = PD.rowwise().sum();
    auto median_idx = sum.minCoeff();
    return V.row(median_idx);
}

}

#endif //BCG_GRAPHICS_BCG_VECTOR_MEDIAN_FILTER_H
