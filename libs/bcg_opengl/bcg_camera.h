//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_CAMERA_H
#define BCG_GRAPHICS_BCG_CAMERA_H

#include "math/bcg_linalg.h"

namespace bcg {

struct camera {
    MatrixS<4, 4> projection_matrix;
    Transform model_matrix;
    VectorS<3> target_point;
    bcg_scalar_t near, far, aspect, fovy;
    bcg_scalar_t left, right, top, bottom;
    bool orthographic;

    camera();

    virtual void init();

    void set_target(const VectorS<3> &target);

    VectorS<3> front_vec();

    VectorS<3> up_vec();

    VectorS<3> left_vec();

    VectorS<3> position();

    MatrixS<4, 4> view_matrix();

    void update_projection(bool orthographic = false);

};

}

#endif //BCG_GRAPHICS_BCG_CAMERA_H
