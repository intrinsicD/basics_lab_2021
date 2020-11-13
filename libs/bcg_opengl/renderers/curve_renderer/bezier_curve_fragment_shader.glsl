#version 440

struct Material {
    vec3 uniform_color;
    float alpha;
};

uniform Material material;

out vec4 final_color;

void main(){
    final_color =  vec4(material.uniform_color, material.alpha);
}