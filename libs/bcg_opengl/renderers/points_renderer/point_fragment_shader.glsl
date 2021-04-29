#version 330

in vec4 f_color;
in vec4 f_position;
in vec3 f_normal;
in mat4 f_proj;
in float f_point_size;
out vec4 final_color;

uniform bool has_normals;

void sphere() {
    float radius = 0.5;
    vec3 sphere_normal = vec3(gl_PointCoord.x - 0.5, gl_PointCoord.y - 0.5, 0.0);
    float len = length(sphere_normal);

    if (len > radius) discard;
    sphere_normal.z = sqrt(1.0 - dot(sphere_normal, sphere_normal));
    vec3 light_dir = normalize(f_position.xyz);
    vec3 view_dir = normalize(-f_position.xyz);
    vec3 normal;
    if (has_normals){
        normal = normalize(f_normal);
    } else {
        normal = sphere_normal;
    }

    float diff = max(abs(dot(normal, light_dir)), 0.0);

    vec3 reflect_dir = reflect(light_dir, normal);
    float spec = pow(max(abs(dot(view_dir, reflect_dir)), 0.0), 5.0);
    float d2 = dot(sphere_normal.xy, sphere_normal.xy);
    float dr =  sqrt(radius * radius - d2);
    final_color = (f_color * diff + f_color * spec) * pow(sphere_normal.z, 20);

    gl_FragDepth = (gl_FragCoord.z / gl_FragCoord.w + dr *  gl_DepthRange.diff / 2.0 * f_proj[2].z) * gl_FragCoord.w;
}


void sphere2() {
    float radius = 0.5;
    vec3 sphere_normal = vec3(gl_PointCoord.x - 0.5, gl_PointCoord.y - 0.5, 0.0);
    float len = length(sphere_normal);

    if (len > radius) discard;
    sphere_normal.z = sqrt(1.0 - dot(sphere_normal, sphere_normal));
    vec3 light_dir = normalize(f_position.xyz);
    vec3 view_dir = normalize(-f_position.xyz);
    vec3 normal;
    if (has_normals){
        normal = normalize(f_normal);
    } else {
        normal = sphere_normal;
    }

    float diff = max(abs(dot(normal, light_dir)), 0.0);

    vec3 reflect_dir = reflect(light_dir, normal);
    float spec = pow(max(abs(dot(view_dir, reflect_dir)), 0.0), 5.0);
    float d2 = dot(sphere_normal.xy, sphere_normal.xy);
    float dr =  sqrt(radius * radius - d2);
    final_color = f_color * diff + f_color * spec;

    vec4 pos = vec4(f_position.xyz, 1.0);
    pos.z += (sphere_normal.z * dr) *  gl_DepthRange.diff / 2.0 * f_proj[2].z  * f_point_size;
    pos = f_proj * pos;
    pos = pos / pos.w;
    gl_FragDepth = (pos.z + 1.0) / 2.0;

    gl_FragDepth = (gl_FragCoord.z / gl_FragCoord.w + (sphere_normal.z * f_point_size ) *  gl_DepthRange.diff / 2.0 * f_proj[2].z) * gl_FragCoord.w;
}


void splat() {
    // code from: High-Quality Point-Based Rendering Using FastSingle-Pass Interpolation
    float u = 2.0 * gl_PointCoord.x - 1.0;
    float v = 2.0 * gl_PointCoord.y - 1.0;
    float w = 1.0 - ( u*u + v*v );

    float len = length(vec2(u, v));

    if (len > 0.5) discard;

    vec4 pos = vec4(f_position.xyz, 1.0);
    pos.z += w * f_point_size;
    pos = f_proj * pos;
    pos = pos / pos.w;
    gl_FragDepth = (pos.z + 1.0) / 2.0;

    vec3 light_dir = normalize(f_position.xyz);
    vec3 view_dir = normalize(-f_position.xyz);
    vec3 normal;
    vec3 sphere_normal = vec3(u, v, w);
    if (has_normals){
        normal = normalize(f_normal);
    } else {
        normal = sphere_normal;
    }

    float diff = max(abs(dot(normal, light_dir)), 0.0);

    vec3 reflect_dir = reflect(light_dir, normal);
    float spec = pow(max(abs(dot(view_dir, reflect_dir)), 0.0), 5.0);
    final_color = f_color * diff + f_color * spec;
}

void main(){
    sphere();
    //sphere2();
    //splat();
}


