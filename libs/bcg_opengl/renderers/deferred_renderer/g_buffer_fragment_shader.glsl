#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec3 FinalPos;
in vec3 Normal;
in vec3 Color;

uniform bool use_uniform_color;
uniform vec3 uniform_color;

void main(){
    // store the fragment position vector in the first gbuffer texture
    gPosition = FinalPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // and the diffuse per-fragment color
    if(use_uniform_color){
        gAlbedoSpec.rgb = uniform_color;
        gAlbedoSpec.a = 0;
    }else{
        gAlbedoSpec.rgb = Color;
        gAlbedoSpec.a = 0;
    }
}