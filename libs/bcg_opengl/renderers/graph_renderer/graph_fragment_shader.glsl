#version 330 core

struct Material {
    bool use_uniform_color;
    vec3 uniform_color;
    float alpha;
};

uniform Material material;

uniform sampler2D edge_color;
uniform int width;

out vec4 final_color;

ivec2 get_tex_coords(int id){
    int i = id % width;
    int j = (id - i) / width;
    return ivec2(i, j);
}

void main(){
    if(material.use_uniform_color){
        final_color = vec4(material.uniform_color, material.alpha);
    }else{
        final_color = vec4(texelFetch(e_color, get_tex_coords(gl_PrimitiveID), 0).xyz, material.alpha);
    }
}