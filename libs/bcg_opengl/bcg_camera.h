//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_CAMERA_H
#define BCG_GRAPHICS_BCG_CAMERA_H

#include "bcg_library/bcg_vector.h"
#include "bcg_library/bcg_matrix.h"

namespace bcg {

struct camera {
    mat4f projection_matrix;
    mat4f model_matrix;
    vec3f target_point;
    float near, far, aspect, fovy;
    bool orthographic;
};

void init_camera(camera &camera, bool orthographic = false);

vec3f inline front(const camera &cam);

vec3f inline up(const camera &cam);

vec3f inline left(const camera &cam);

vec3f inline position(const camera &cam);

mat4f inline view_matrix(const camera &cam);

void update_projection(camera &cam, bool orthographic = false);

}

namespace bcg{

void init_camera(camera &camera, bool orthographic){
    camera.target_point = zero3f;
    camera.near = 0.01;
    camera.far  = 10.0;
    camera.aspect = 1.0;
    camera.fovy = 45.0;
    update_projection(camera, orthographic);
}

vec3f inline front(const camera &cam){
    return xyz(cam.model_matrix.z);
}

vec3f inline up(const camera &cam){
    return xyz(cam.model_matrix.y);
}

vec3f inline left(const camera &cam){
    return xyz(cam.model_matrix.x);
}

vec3f inline position(const camera &cam){
    return xyz(cam.model_matrix.w);
}

mat4f inline view_matrix(const camera &cam){
    return inverse()
}

void update_projection(camera &cam, bool orthographic){

}

}

#endif //BCG_GRAPHICS_BCG_CAMERA_H
