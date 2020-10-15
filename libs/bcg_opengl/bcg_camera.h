//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_CAMERA_H
#define BCG_GRAPHICS_BCG_CAMERA_H

#include "math/bcg_linalg.h"

namespace bcg {

struct camera {
    MatrixS<4, 4> projection_matrix;
    MatrixS<4, 4> model_matrix;
    VectorS<3> target_point;
    bcg_scalar_t near, far, aspect, fovy;

    virtual void init();

    VectorS<3> inline front();

    VectorS<3> inline up();

    VectorS<3> inline left();

    VectorS<3> inline position();

    MatrixS<4, 4> inline view_matrix();

    virtual void update_projection() {};

};

struct perspective_camera : public camera {
    void init() override;

    inline void update_projection() override;
};

struct orthographic_camera : public camera {
    bcg_scalar_t l, r, t, b;

    void init() override;

    inline void update_projection() override;
};

}

namespace bcg {

void camera::init() {
    target_point = zero3s;
    near = 0.01;
    far = 10.0;
    aspect = 1.0;
    fovy = 45.0;
    update_projection();
}

VectorS<3> inline camera::front() {
    return VectorS<3>(model_matrix[2]);
}

VectorS<3> inline camera::up() {
    return VectorS<3>(model_matrix[1]);
}

VectorS<3> inline camera::left() {
    return VectorS<3>(model_matrix[0]);
}

VectorS<3> inline camera::position() {
    return VectorS<3>(model_matrix[3]);
}

MatrixS<4, 4> inline camera::view_matrix() {
    return model_matrix.inverse();
}

void perspective_camera::init() {
    camera::init();
}

void perspective_camera::update_projection() {
    projection_matrix = glm::perspective(fovy, aspect, near, far);
}

void orthographic_camera::init() {
    l = -1;
    r = 1;
    t = 1;
    b = -1;
    camera::init();
}

void orthographic_camera::update_projection() {
    projection_matrix = glm::ortho(l, r, t, b, near, far);
}

}

#endif //BCG_GRAPHICS_BCG_CAMERA_H
