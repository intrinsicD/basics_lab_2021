#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform float uniform_point_size;

void main() {
    gl_PointSize = uniform_point_size;
    gl_Position = proj * view * model * vec4(position, 1.0f);
}
