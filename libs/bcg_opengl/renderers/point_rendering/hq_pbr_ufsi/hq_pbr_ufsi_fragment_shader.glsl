#version 330
//High-Quality Point-Based Rendering Using FastSingle-Pass Interpolation

in vec4 f_color;
in vec3 f_normal;
in vec4 view_pos;
in float v_radius;

uniform mat4 proj;

out vec4 final_color;

void main() {
    float u = 2.0 * gl_PointCoord.x - 1.0;
    float v = 2.0 * gl_PointCoord.y - 1.0;
    float w = 1.0 - (u * u + v * v);

    vec4 pos = vec4(view_pos.xyz, 1.0);
    pos.z += w * float(v_radius);
    pos = proj * pos;
    pos = pos / pos.w;

    gl_FragDepth = (pos.z + 1.0) / 2.0;

    final_color = vec4(f_color.xyz * w * abs(f_normal.z), f_color.w);
}
