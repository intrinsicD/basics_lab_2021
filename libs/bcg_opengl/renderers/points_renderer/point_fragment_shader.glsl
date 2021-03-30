#version 330

in vec4 f_color;
in vec4 f_position;
in vec3 f_normal;
in mat4 f_proj;
out vec4 final_color;

uniform bool has_normals;

void main() {
    float radius = 0.5;
    vec3 sphere_normal = vec3(gl_PointCoord.x - 0.5, gl_PointCoord.y - 0.5, 0.0);
    float len = length(sphere_normal);

    if (len > radius) discard;
    sphere_normal.z = -1.0;
    vec3 light_dir = normalize(f_position.xyz);
    vec3 view_dir = normalize(-f_position.xyz);
    vec3 normal;
    if(has_normals){
        normal = normalize(f_normal);
    }else{
        normal = normalize(sphere_normal);
    }

    float diff = max(abs(dot(normal, light_dir)), 0.0);

    vec3 reflect_dir = reflect(light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 5.0);
    float d2 = dot(sphere_normal.xy, sphere_normal.xy);
    float dr =  sqrt(radius - d2);
    final_color = f_color * diff + f_color * spec;
    gl_FragDepth = (gl_FragCoord.z / gl_FragCoord.w + dr *  gl_DepthRange.diff / 2.0 * f_proj[2].z) * gl_FragCoord.w;
}
