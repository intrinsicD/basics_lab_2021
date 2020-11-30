//
// Created by alex on 30.11.20.
//

#include "bcg_projection_matrix.h"

namespace bcg{


    projection::projection() : matrix(MatrixS<4, 4>::Identity()) {}

    void projection::perspective(bcg_scalar_t near, bcg_scalar_t far, bcg_scalar_t aspect, bcg_scalar_t fovy_radians) {
        bcg_scalar_t top = near * std::tan(fovy_radians / 2.0);
        bcg_scalar_t bottom = -top;
        bcg_scalar_t left = bottom * aspect;
        bcg_scalar_t right = top * aspect;
        perspective(near, far, top, bottom, left, right);
    }

    void projection::perspective(bcg_scalar_t near, bcg_scalar_t far, bcg_scalar_t top, bcg_scalar_t bottom, bcg_scalar_t left, bcg_scalar_t right) {
        matrix(0, 0) = (near + near) / (right - left);
        matrix(0, 2) = (right + left) / (right - left);
        matrix(1, 1) = (near + near) / (top - bottom);
        matrix(1, 2) = (top + bottom) / (top - bottom);
        matrix(2, 2) = -(far + near) / (far - near);
        matrix(2, 3) = -2.0 * far * near / (far - near);
        matrix(3, 2) = -1.0;
        matrix(3, 3) = 0.0;
    }

    void projection::orthographic(bcg_scalar_t near, bcg_scalar_t far, bcg_scalar_t aspect, bcg_scalar_t fovy_radians) {
/*        bcg_scalar_t min = -fovy_radians;
        bcg_scalar_t max = fovy_radians;
        bcg_scalar_t left = min * aspect;
        bcg_scalar_t right = max * aspect;
        bcg_scalar_t top = max;
        bcg_scalar_t bottom = min;*/

        bcg_scalar_t ratio_size_per_depth = std::tan(fovy_radians / 2.0);
        bcg_scalar_t size_y = ratio_size_per_depth * far;
        bcg_scalar_t size_x = ratio_size_per_depth * far * aspect;
        bcg_scalar_t left = -size_x;
        bcg_scalar_t right = size_x;
        bcg_scalar_t bottom = -size_y;
        bcg_scalar_t top = size_y;
        orthographic(near, 2.0 * far, top, bottom, left, right);
    }

    void projection::orthographic(bcg_scalar_t near, bcg_scalar_t far, bcg_scalar_t top, bcg_scalar_t bottom, bcg_scalar_t left, bcg_scalar_t right) {
        matrix(0, 0) = 2.0 / (right - left);
        matrix(1, 1) = 2.0 / (top - bottom);
        matrix(2, 2) = -2.0 / (far - near);
        matrix(0, 3) = -(right + left) / (right - left);
        matrix(1, 3) = -(top + bottom) / (top - bottom);
        matrix(2, 3) = -(far + near) / (far - near);
        matrix(3, 3) = 1.0;
    }

}