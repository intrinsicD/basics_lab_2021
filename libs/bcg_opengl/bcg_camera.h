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
    bcg_scalar_t near, far, aspect, fovy_degrees;
    bcg_scalar_t left, right, top, bottom;
    bool orthographic;

    VectorS<3> last_point_3d;
    bool last_point_ok;
    bcg_scalar_t rot_speed;

    camera();

    virtual void init();

    void set_target(const VectorS<3> &target);

    bcg_scalar_t fovy_radians() const;

    VectorS<3> front_vec() const;

    VectorS<3> up_vec() const;

    VectorS<3> right_vec() const;

    VectorS<3> position() const;

    MatrixS<4, 4> view_matrix() const;

    void update_projection(bool orthographic = false);

};

Translation world_translation(const VectorS<2> &delta_pos, int width, int height, camera &cam);

Rotation world_rotation(const VectorS<2> &pos, int width, int height, camera &cam);

}

#endif //BCG_GRAPHICS_BCG_CAMERA_H
