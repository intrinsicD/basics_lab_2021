#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in float point_size;
layout (location = 3) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec4 viewport;

struct Material{
    bool use_uniform_color;
    vec3 uniform_color;

    bool use_uniform_point_size;
    bool has_normals;
    float uniform_point_size;
    float alpha;
};

uniform Material material;

out vec4 f_color;
out vec3 f_normal;
out mat4 f_proj;
out vec4 f_position;
out float f_point_size;
uniform bool has_normals;
uniform float fovy;

vec3 transform_normals(vec3 n){
    return mat3(transpose(inverse(view * model))) * n;
}

void main() {
    if(material.use_uniform_color){
        f_color = vec4(material.uniform_color, material.alpha);
    }else{
        f_color = vec4(color, material.alpha);
    }
    f_position = view * model * vec4(position, 1.0f);

    if(material.use_uniform_point_size){
        gl_PointSize = material.uniform_point_size;
    }else{
        gl_PointSize = point_size * material.uniform_point_size * 2;
    }

    if(has_normals){
        f_normal = transform_normals(normal);
    }
    f_proj = proj;
    gl_Position = proj * f_position;
    //gl_Position.z *= 0.999999;

    // code from: High-Quality Point-Based Rendering Using FastSingle-Pass Interpolation
    float projFactor = 1.0 / tan(fovy / 2.0);
    projFactor = projFactor / -f_position.z;
    projFactor = projFactor * viewport[3] / 2.0;

    //f_point_size = gl_PointSize / projFactor;
    f_point_size = 1.0 / projFactor;
}
