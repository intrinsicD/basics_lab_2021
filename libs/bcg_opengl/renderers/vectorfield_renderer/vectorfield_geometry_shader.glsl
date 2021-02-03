#version 330 core
layout(points) in;
layout(line_strip, max_vertices=2) out;

struct Material {
    bool use_uniform_color;
    vec3 uniform_color;

    bool use_uniform_vector_length;
    float uniform_vector_length;
    float alpha;
};

uniform Material material;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

in Vertex{
    vec3 vector;
    vec3 color;
} vertex[];

out vec4 vertex_color;

void EmitVertexVector(int Index){
    vec3 P = gl_in[Index].gl_Position.xyz;
    vec3 V = vertex[Index].vector;


    if(material.use_uniform_vector_length){
        V = normalize(vertex[Index].vector) * material.uniform_vector_length;
    }else{
        V = vertex[Index].vector * material.uniform_vector_length;
    }

    if(material.use_uniform_color){
        vertex_color = vec4(material.uniform_color, material.alpha);
    }else{
        vertex_color = vec4(vertex[Index].color, material.alpha);
    }

    gl_Position = proj * view * model * vec4(P, 1.0);
    gl_Position.z *= 0.999999;
    EmitVertex();

    gl_Position =  proj * view * model * vec4(P + V, 1.0);
    gl_Position.z *= 0.999999;
    EmitVertex();

    EndPrimitive();
}

void main(){
    for (int i = 0; i < gl_in.length(); i++){
        EmitVertexVector(i);
    }
}