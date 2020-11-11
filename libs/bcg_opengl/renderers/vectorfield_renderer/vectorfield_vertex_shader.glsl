#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vector;
layout (location = 2) in vec3 color;

out Vertex{
    vec3 vector;
    vec3 color;
} vertex;

void main(){
    gl_Position = vec4(position, 1.0f);
    vertex.vector = vector;
}
