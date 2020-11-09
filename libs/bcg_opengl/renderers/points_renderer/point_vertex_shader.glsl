#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 points_color;
layout (location = 2) in float point_size;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

struct Material{
    bool use_uniform_color;
    vec3 uniform_color;

    bool use_uniform_point_size;
    float uniform_point_size;
    float alpha;
};

uniform Material material;

out vec4 f_color;

void main() {
    if(material.use_uniform_color){
        f_color = vec4(material.uniform_color, material.alpha);
    }else{
        f_color = vec4(points_color, material.alpha);
    }

    if(material.use_uniform_point_size){
        gl_PointSize = material.uniform_point_size;
    }else{
        gl_PointSize = point_size * material.uniform_point_size;
    }

    gl_Position = proj * view * model * vec4(position, 1.0f);
}
