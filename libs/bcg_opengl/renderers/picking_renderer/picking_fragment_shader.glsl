#version 330 core

out vec4 final_color;

struct Material {
    vec3 picking_color;
};

uniform Material material;

void main(){
    final_color = vec4(material.picking_color, 1.0f);
}