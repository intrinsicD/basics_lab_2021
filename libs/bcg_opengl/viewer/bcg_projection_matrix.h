//
// Created by alex on 27.10.20.
//

#ifndef BCG_GRAPHICS_BCG_PROJECTION_MATRIX_H
#define BCG_GRAPHICS_BCG_PROJECTION_MATRIX_H

#include "math/matrix/bcg_matrix.h"

namespace bcg{

struct projection{
    MatrixS<4, 4> matrix;
    bcg_scalar_t n, f, t, b, l, r;

    projection();

    void perspective(bcg_scalar_t near, bcg_scalar_t far, bcg_scalar_t aspect, bcg_scalar_t fovy_radians);

    void perspective(bcg_scalar_t near, bcg_scalar_t far, bcg_scalar_t top, bcg_scalar_t bottom, bcg_scalar_t left, bcg_scalar_t right);

    void orthographic(bcg_scalar_t near, bcg_scalar_t far, bcg_scalar_t aspect, bcg_scalar_t fovy_radians);

    void orthographic(bcg_scalar_t near, bcg_scalar_t far, bcg_scalar_t top, bcg_scalar_t bottom, bcg_scalar_t left, bcg_scalar_t right) ;
};

}

#endif //BCG_GRAPHICS_BCG_PROJECTION_MATRIX_H
