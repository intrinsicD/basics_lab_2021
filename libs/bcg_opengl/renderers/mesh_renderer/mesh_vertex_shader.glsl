#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 mesh_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 f_normal;
out vec3 f_color;
out vec4 f_position;

vec3 transform_normals(vec3 n){
    return mat3(transpose(inverse(view * model))) * n;
}

void main() {
    f_normal = transform_normals(normal);
    f_color = mesh_color;
    f_position = view * model * vec4(position, 1.0f);
    gl_Position = proj * f_position;
}
