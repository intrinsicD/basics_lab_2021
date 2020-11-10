#version 330

in vec4 f_color;
out vec4 final_color;

void main() {
    float radius = 0.5;
    vec3 sphere_normal = vec3(gl_PointCoord.x - 0.5, gl_PointCoord.y - 0.5, 0);
    float len = length(sphere_normal);
    if(len > radius) discard;
    final_color = f_color;
}
