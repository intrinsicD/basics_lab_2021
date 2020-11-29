#version 330
in vec3 f_normal;
in vec3 f_color;
in vec4 f_position;

struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float alpha;
    bool use_uniform_color;
    bool use_face_color;
};

uniform Material material;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform int width;
uniform sampler2D face_color;

out vec4 final_color;

vec4 phong_shading(Light light, Material material, vec3 color, bool use_uniform_color){
    //ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse
    vec3 normal = normalize(f_normal);
    vec3 light_dir = normalize(light.position - f_position.xyz);
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // specular
    vec3 view_dir = normalize(- f_position.xyz);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec4 result_color;
    if (!use_uniform_color){
        result_color = vec4(ambient + color + specular, material.alpha);
    } else {
        result_color = vec4(ambient + diffuse + specular, material.alpha);
    }
    return result_color;
}

ivec2 get_tex_coords(int id){
    int i = id % width;
    int j = (id - i) / width;
    return ivec2(i, j);
}

void main() {
    Light light;
    light.position = vec3(0, 0, 0);
    light.ambient = vec3(0.2, 0.2, 0.2);
    light.diffuse = vec3(0.5, 0.5, 0.5);
    light.specular = vec3(1, 1, 1);

    if(material.use_face_color){
        //final_color = vec4(texelFetch(face_color, get_tex_coords(gl_PrimitiveID), 0).xyz, material.alpha);
        bool use_uniform_color = false;
        vec3 color = texelFetch(face_color, get_tex_coords(gl_PrimitiveID), 0).xyz;
        final_color = phong_shading(light, material, color, use_uniform_color);
    }else{
        final_color = phong_shading(light, material, f_color, material.use_uniform_color);
    }
}
