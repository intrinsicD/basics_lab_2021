//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_ROTATION_MATRIX_EXPONENTIAL_H
#define BCG_GRAPHICS_BCG_ROTATION_MATRIX_EXPONENTIAL_H

#include "bcg_rotation_cross_product_matrix.h"

namespace bcg {

inline MatrixS<3, 3> matrix_exponential(const VectorS<3> &v) {
    //Rodrigues' fromula
    bcg_scalar_t theta = v.norm();
    bcg_scalar_t theta_squared = theta * theta;

    bcg_scalar_t first_term;
    bcg_scalar_t second_term;
    if(theta > scalar_eps){
        first_term = std::sin(theta) / theta;
        second_term = (1.0 - std::cos(theta)) / theta_squared;
    }else{
        first_term = 1.0 - theta_squared / 6.0 * (1.0 - theta_squared / 20.0 * (1.0 - theta_squared / 42.0));
        second_term = (1.0 - theta_squared / 12.0 * (1.0 - theta_squared / 30.0 * (1.0 - theta_squared / 56.0))) / 2.0;
    }
    MatrixS<3, 3> rot(cross_product_matrix(v));
    return MatrixS<3, 3>::Identity() + first_term * rot + second_term * rot * rot;
}

}

#endif //BCG_GRAPHICS_BCG_ROTATION_MATRIX_EXPONENTIAL_H
