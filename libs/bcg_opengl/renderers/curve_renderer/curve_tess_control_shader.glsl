#version 410 core

layout (vertices = 4) out;

uniform int tesselation_level;

void main(){
    gl_TessLevelOuter[0] = 1;
    gl_TessLevelOuter[1] = tesselation_level;

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}