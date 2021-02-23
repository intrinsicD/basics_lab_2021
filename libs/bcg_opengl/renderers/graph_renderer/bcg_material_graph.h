//
// Created by alex on 10.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_GRAPH_H
#define BCG_GRAPHICS_BCG_MATERIAL_GRAPH_H

#include "color/bcg_colormap.h"
#include "renderers/bcg_material.h"

namespace bcg{

struct material_graph : public material{
    material_graph() {
        attributes = {
                {"position", "v_position", "v_position", 0, true},
                {"edge_color", "", "", -1, true}
        };
        textures.emplace_back("edge_color");
    }

    ogl_vertex_array vao;

    bool use_uniform_color = true;
    int width;

    Vector<float, 3> uniform_color = color<float>::red;
    float uniform_alpha = 1;
    colormap::base_colormap color_map;

    ogl_texture &edge_colors(){
        return textures[0];
    }

    void upload(const glsl_program &program) override {
        program.set_uniform_i("material.use_uniform_color", use_uniform_color);
        program.set_uniform_3f("material.uniform_color", 1, uniform_color.data());
        program.set_uniform_f("material.alpha", uniform_alpha);
        program.set_uniform_i("width", width);
        material::upload(program);
    }
};

}

#endif //BCG_GRAPHICS_BCG_MATERIAL_GRAPH_H
