#version 410 core

layout (isolines, equal_spacing, ccw) in;

uniform mat4  model;
uniform mat4  view;
uniform mat4  proj;

uniform bool show_bezier;
uniform bool show_hermite;

uniform sampler2D control_points;
uniform int width;

ivec2 get_tex_coords(int id){
    int i = id % width;
    int j = (id - i) / width;
    return ivec2(i, j);
}

///////////////////////////////////////////////////
// function to calculate the hermite function value
vec3 hermite(float u, vec3 p0, vec3 p1, vec3 t0, vec3 t1)
{
    float F1 = 2.*u*u*u - 3.*u*u + 1.;
    float F2 = -2.*u*u*u + 3*u*u;
    float F3 = u*u*u - 2.*u*u + u;
    float F4 = u*u*u - u*u;

    vec3 p = F1*p0 + F2*p1 + F3*t0 + F4*t1;
    return p;
}

///////////////////////////////////////////////////
// function to evaluate  the Bezier curve
vec3 bezier(float u, vec3 p0, vec3 p1, vec3 p2, vec3 p3)
{
    float B0 = (1.-u)*(1.-u)*(1.-u);
    float B1 = 3.*u*(1.-u)*(1.-u);
    float B2 = 3.*u*u*(1.-u);
    float B3 = u*u*u;

    vec3 p = B0*p0 + B1*p1 + B2*p2 + B3*p3;
    return p;
}

void main(){
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 pos = vec4(0.0, 0.0 ,0.0 ,1.0);
    if(show_hermite){
        vec3 p0 = vec3( gl_in[0].gl_Position );
        vec3 m0 = vec3( gl_in[1].gl_Position ) - p0;
        vec3 p1 = vec3( gl_in[3].gl_Position );
        vec3 m1 = p1 - vec3( gl_in[2].gl_Position );
        pos = vec4(hermite(u, p0, p1, m0, m1), 1.);
    }else if(show_bezier){
        vec3 p0 = vec3(gl_in[0].gl_Position);
        vec3 p1 = vec3(gl_in[1].gl_Position);
        vec3 p2 = vec3(gl_in[2].gl_Position);
        vec3 p3 = vec3(gl_in[3].gl_Position);
        pos = vec4(bezier(u, p0, p1, p2, p3), 1.);
    }
    gl_Position = proj * view * model * pos;
    gl_Position.z *= 0.9999999;
}