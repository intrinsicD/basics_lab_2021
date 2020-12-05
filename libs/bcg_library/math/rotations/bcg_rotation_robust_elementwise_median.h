//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_ROTATION_ROBUST_ELEMENTWISE_MEDIAN_H
#define BCG_GRAPHICS_BCG_ROTATION_ROBUST_ELEMENTWISE_MEDIAN_H

#include "math/matrix/bcg_matrix_elementwise_median.h"
#include "math/rotations/bcg_rotation_project_on_so.h"

namespace bcg{

template<int N>
inline MatrixS<N, N> robust_elementwise_median(const std::vector<MatrixS<N, N>> &rotations) {
    return project_on_so(elementwise_median(rotations), true);
}

}

#endif //BCG_GRAPHICS_BCG_ROTATION_ROBUST_ELEMENTWISE_MEDIAN_H
