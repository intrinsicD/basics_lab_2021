//
// Created by alex on 27.10.20.
//

#include "bcg_camera.h"

namespace bcg {

camera::camera() : projection_matrix(MatrixS<4, 4>::Identity()),
                   model_matrix(Transform::Identity()),
                   target_point(zero3s),
                   near(0.01),
                   far(10.0),
                   aspect(1.0),
                   fovy(45.0) {

}

void camera::init() {
    target_point = zero3s;
    near = 0.01;
    far = 10.0;
    aspect = 1.0;
    fovy = 45.0;
    update_projection();
}

void camera::set_target(const VectorS<3> &target) {
    target_point = target;
    VectorS<3> diff = target_point - position();
    VectorS<3> front = front_vec();
    model_matrix = Translation(diff - front * front.dot(diff)) * model_matrix;
}

VectorS<3> camera::front_vec() {
    return model_matrix.matrix().col(2).head<3>();
}

VectorS<3> camera::up_vec() {
    return model_matrix.matrix().col(1).head<3>();
}

VectorS<3> camera::left_vec() {
    return model_matrix.matrix().col(0).head<3>();
}

VectorS<3> camera::position() {
    return model_matrix.translation();
}

MatrixS<4, 4> camera::view_matrix() {
    return model_matrix.inverse().matrix();
}

void perspective_camera::init() {
    camera::init();
}

void perspective_camera::update_projection() {
    assert(std::abs(aspect - scalar_eps) > 0);

    const auto tanHalfFovy = std::tan(fovy / 2.0);
    projection_matrix.setIdentity();
    projection_matrix(0, 0) = 1.0 / (aspect * tanHalfFovy);
    projection_matrix(1, 1) = 1.0 / (tanHalfFovy);
    projection_matrix(2, 2) = -(far + near) / (far - near);
    projection_matrix(2, 3) = -1.0;
    projection_matrix(3, 2) = -(2.0 * far * near) / (far - near);
}

void orthographic_camera::init() {
    left = -1;
    right = 1;
    top = 1;
    bottom = -1;
    camera::init();
}

void orthographic_camera::update_projection() {
    projection_matrix.setIdentity();
    projection_matrix(0, 0) = 2.0 / (right - left);
    projection_matrix(1, 1) = 2.0 / (top - bottom);
    projection_matrix(2, 2) = -2.0 / (far - near);
    projection_matrix(3, 0) = -(right + left) / (right - left);
    projection_matrix(3, 1) = -(top + bottom) / (top - bottom);
    projection_matrix(3, 2) = -(far + near) / (far - near);
}


}