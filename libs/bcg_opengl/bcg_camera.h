//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_CAMERA_H
#define BCG_GRAPHICS_BCG_CAMERA_H

#include "bcg_library/math/bcg_linalg.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"

namespace bcg {

struct camera {
    mat4f projection_matrix;
    mat4f model_matrix;
    vec3f target_point;
    float near, far, aspect, fovy;

    virtual void init();

    vec3f inline front();

    vec3f inline up();

    vec3f inline left();

    vec3f inline position();

    mat4f inline view_matrix();

    virtual void update_projection() {};

};

struct perspective_camera : public camera {
    void init() override;

    inline void update_projection() override;
};

struct orthographic_camera : public camera {
    float l, r, t, b;

    void init() override;

    inline void update_projection() override;
};

}

namespace bcg {

void camera::init() {
    target_point = zero3f;
    near = 0.01;
    far = 10.0;
    aspect = 1.0;
    fovy = 45.0;
    update_projection();
}

vec3f inline camera::front() {
    return vec3f(model_matrix[2]);
}

vec3f inline camera::up() {
    return vec3f(model_matrix[1]);
}

vec3f inline camera::left() {
    return vec3f(model_matrix[0]);
}

vec3f inline camera::position() {
    return vec3f(model_matrix[3]);
}

mat4f inline camera::view_matrix() {
    return glm::inverse(model_matrix);
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
