#version 330
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_color;
layout (location = 2) in float v_point_size;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform bool use_uniform_color;
uniform vec4 uniform_color;

uniform bool use_uniform_point_size;
uniform float uniform_point_size;

out vec4 f_color;

void main() {
    if(use_uniform_color){
        f_color = uniform_color;
    }else{
        f_color = vec4(v_color, 1.0f);
    }

    if(use_uniform_point_size){
        gl_PointSize = uniform_point_size;
    }else{
        gl_PointSize = v_point_size;
    }

    gl_Position = proj * view * model * vec4(v_position, 1.0f);
}
