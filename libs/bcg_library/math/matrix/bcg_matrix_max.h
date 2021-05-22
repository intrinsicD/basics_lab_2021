//
// Created by alex on 20.05.21.
//

#ifndef BCG_GRAPHICS_BCG_MATRIX_MAX_H
#define BCG_GRAPHICS_BCG_MATRIX_MAX_H

#include "bcg_matrix.h"
#include "Eigen/Dense"

namespace bcg {

template<typename Derived>
Vector<typename Derived::Scalar, -1> mat_max(const Ref<Derived> &matrix, int dim, VectorI<-1> &max_indices) {
    using Scalar = typename Derived::Scalar;

    assert(dim == 1 || dim == 2);

    // output size
    bcg_index_t n = (dim == 1 ? matrix.cols() : matrix.rows());
    // resize output
    Vector<Scalar, -1> max_values;
    max_values.resize(n);
    max_indices.resize(n);

    // loop over dimension opposite of dim
    for (bcg_index_t j = 0; j < n; ++j) {
        if (dim == 1) {
            max_values(j) = matrix.col(j).maxCoeff(&max_indices(j));
        } else {
            max_values(j) = matrix.row(j).maxCoeff(&max_indices(j));
        }
    }
    return max_values;
}

}

#endif //BCG_GRAPHICS_BCG_MATRIX_MAX_H
