//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_MESH_H
#define BCG_GRAPHICS_BCG_MATERIAL_MESH_H

#include "color/bcg_colormap.h"
#include "renderers/bcg_material.h"

namespace bcg{

struct material_mesh : public material{
    material_mesh() {
        attributes = {
                {"position", "v_position","v_position", 0, true},
                {"normal", "v_normal", "v_normal", 1, true},
                {"color", "", "", 2, false}
        };
        textures.emplace_back("face_color");
    }

    ogl_vertex_array vao;

    bool use_uniform_color = true;
    bool use_face_color = false;

    Vector<float, 3> ambient = color<float>::random();
    Vector<float, 3> diffuse = color<float>::white;
    Vector<float, 3> specular = color<float>::grey;
    float shininess = 32;
    float uniform_alpha = 1;

    colormap::base_colormap color_map = colormap::jet();
    int width;

    ogl_texture &face_colors(){
        return textures[0];
    }

    void upload(const glsl_program &program) override {
        program.set_uniform_i("material.use_uniform_color", use_uniform_color);
        program.set_uniform_3f("material.ambient", 1, ambient.data());
        program.set_uniform_3f("material.diffuse", 1, diffuse.data());
        program.set_uniform_3f("material.specular", 1, specular.data());
        program.set_uniform_f("material.shininess", shininess);
        program.set_uniform_f("material.alpha", uniform_alpha);
        program.set_uniform_i("material.use_face_color", use_face_color);
        program.set_uniform_i("width", width);
        material::upload(program);
    }
};

}

#endif //BCG_GRAPHICS_BCG_MATERIAL_MESH_H
