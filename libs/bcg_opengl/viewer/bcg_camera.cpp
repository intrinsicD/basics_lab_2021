//
// Created by alex on 27.10.20.
//

#include <iostream>
#include "bcg_camera.h"
#include "math/bcg_math_safe_tigonometric_functions.h"

namespace bcg {

camera::camera() : proj(),
                   model_matrix(Translation(0.0, 0.0, 8.0)),
                   target_point(zero3s),
                   near(0.01),
                   far(10.0),
                   aspect(1.0),
                   fovy_degrees(45.0),
                   left(-1),
                   right(1),
                   top(1),
                   bottom(-1),
                   orthographic(false),
                   last_point_3d(0, 0, 0),
                   last_point_ok(false),
                   rot_speed(5),
                   mov_speed(1){

}

void camera::init(int width, int height) {
    near = 0.1;
    far = 200.0;
    aspect = (bcg_scalar_t) width / (bcg_scalar_t) height;
    fovy_degrees = 45.0;
    left = -1;
    right = 1;
    top = 1;
    bottom = -1;
    model_matrix = Translation(0.0, 0.0, 8.0);
    set_target({0, 0, 0});
    update_projection();
}

void camera::set_target(const VectorS<3> &target) {
    target_point = target;
    VectorS<3> diff = target_point - position();
    VectorS<3> front = direction_vec();
    model_matrix = Translation(diff - front * front.dot(diff)) * model_matrix;
}

bcg_scalar_t camera::fovy_radians() const {
    return fovy_degrees * pi / 360.0;
}

VectorS<3> camera::direction_vec() const {
    return model_matrix.matrix().col(2).head<3>();
}

VectorS<3> camera::up_vec() const {
    return model_matrix.matrix().col(1).head<3>();
}

VectorS<3> camera::right_vec() const {
    return model_matrix.matrix().col(0).head<3>();
}

VectorS<3> camera::position() const {
    return model_matrix.translation();
}

MatrixS<4, 4> camera::view_matrix() const {
    return model_matrix.inverse().matrix();
}

MatrixS<4, 4> camera::projection_matrix() const{
    return proj.matrix;
}

void camera::update_projection() {
    if (orthographic) {
        proj.orthographic(0, (target_point - position()).norm(), aspect, fovy_radians());
    } else {
        proj.perspective(near, far, aspect, fovy_radians());
    }
}

bool map_to_sphere(const VectorS<2> &point, int width, int height, VectorS<3> &result) {
    if ((point[0] >= 0) && (point[0] <= bcg_scalar_t(width)) && (point[1] >= 0) && (point[1] <= bcg_scalar_t(height))) {
        VectorS<2> ndc({(point[0] / bcg_scalar_t(width) - 0.5), (0.5 - point[1] / bcg_scalar_t(height))});
        result[0] = std::sin(pi * ndc[0] * 0.5);
        result[1] = std::sin(pi * ndc[1] * 0.5);
        bcg_scalar_t sinx2siny2 = result[0] * result[0] + result[1] * result[1];
        result[2] = sinx2siny2 < 1.0 ? std::sqrt(1.0 - sinx2siny2) : 0.0;
        return true;
    }
    return false;
}

Translation world_translation(const VectorS<2> &delta_pos, int width, int height, camera &cam) {
    //transform target to view coords
    VectorS<4> ec = cam.view_matrix() * cam.target_point.homogeneous();
    auto z = (ec[2] / ec[3]);

    auto up = std::tan(cam.fovy_radians()) * cam.near;
    auto right = cam.aspect * up;

    return Translation(cam.model_matrix.linear() *
                       VectorS<3>(2.0 * delta_pos[0] / (bcg_scalar_t) width * right / cam.near * z,
                                  -2.0 * delta_pos[1] / (bcg_scalar_t) height * up / cam.near * z,
                                  0.0));
}

Rotation world_rotation(const VectorS<2> &position, int width, int height, camera &cam) {
    VectorS<3> axis(one3s);
    axis.normalize();
    bcg_scalar_t angle = 0;

    if (cam.last_point_ok) {
        VectorS<3> new_point_3d;
        if (map_to_sphere(position, width, height, new_point_3d)) {
            //transform axis to world coords
            axis = (cam.model_matrix.linear() * (cam.last_point_3d.cross(new_point_3d)).normalized()).normalized();
            angle = safe_acos(std::min<bcg_scalar_t>(1.0, cam.last_point_3d.dot(new_point_3d))) * cam.rot_speed;
        }
    }
    return {angle, axis};
}

}