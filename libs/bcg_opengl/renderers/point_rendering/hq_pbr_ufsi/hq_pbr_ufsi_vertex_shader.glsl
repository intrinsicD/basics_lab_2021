#version 330
//High-Quality Point-Based Rendering Using FastSingle-Pass Interpolation

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform int screen_height;
uniform float field_of_view_radians;

struct Material{
    bool use_uniform_color;
    vec3 uniform_color;
    float uniform_size;
    float alpha;
};

uniform Material material;

out vec4 f_color;
out vec3 f_normal;
out vec4 view_pos;
out float v_radius;

vec3 transform_normals(vec3 n){
    return mat3(transpose(inverse(view * model))) * n;
}

void main() {
    view_pos = view * model * vec4(position, 1.0);

    float proj_factor = 1.0 / tan(field_of_view_radians / 2.0);
    proj_factor = proj_factor / -view_pos.z;
    proj_factor = proj_factor * float(screen_height) / 2.0;
    gl_PointSize = material.uniform_size;
    v_radius = gl_PointSize / proj_factor;

    gl_Position = proj * view_pos;

    if (material.use_uniform_color){
        f_color = vec4(material.uniform_color, material.alpha);
    } else {
        f_color = vec4(color, material.alpha);
    }

    f_normal = transform_normals(normal);
}
