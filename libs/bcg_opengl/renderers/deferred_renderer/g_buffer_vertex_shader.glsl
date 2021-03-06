#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;

void main(){
    vec4 worldPos = model * vec4(position, 1.0);

    FragPos = worldPos.xyz;
    Normal = transpose(inverse(mat3(model))) * normal;
    Color = color;

    gl_Position = proj * view * worldPos;
}