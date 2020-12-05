//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_ROTATION_RANDOM_H
#define BCG_GRAPHICS_BCG_ROTATION_RANDOM_H

#include "math/matrix/bcg_matrix.h"
#include "bcg_rotation_matrix_exponential.h"

namespace bcg {

inline MatrixS<3, 3> random_rotation(bcg_scalar_t degree_tol = 3) {
    VectorS<3> vec = VectorS<3>::Random();
    bcg_scalar_t angle_degrees = vec.norm() * degree_tol;
    return matrix_exponential(vec.normalized() * (std::abs(angle_degrees) > degree_tol ? degree_tol : angle_degrees) * pi / 180.0);
}

}

#endif //BCG_GRAPHICS_BCG_ROTATION_RANDOM_H
