//
// Created by alex on 10.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_GRAPH_H
#define BCG_GRAPHICS_BCG_MATERIAL_GRAPH_H

#include "bcg_opengl.h"
#include "color/bcg_colors.h"
#include "renderers/bcg_attribute.h"

namespace bcg{

struct material_graph{
    std::vector<attribute> attributes = {
            {"position", "v_position", "v_position", 0, true},
            {"edge_color", "", "", -1, true}
    };

    ogl_vertex_array vao;

    bool use_uniform_color = true;
    int width;

    VectorS<3> uniform_color = color::red;
    bcg_scalar_t uniform_alpha = 1;
    ogl_texture edge_colors = ogl_texture("edge_color");
    colormap::base_colormap color_map;
};

}

#endif //BCG_GRAPHICS_BCG_MATERIAL_GRAPH_H
