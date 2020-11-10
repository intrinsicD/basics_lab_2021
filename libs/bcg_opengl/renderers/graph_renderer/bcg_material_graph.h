//
// Created by alex on 10.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_GRAPH_H
#define BCG_GRAPHICS_BCG_MATERIAL_GRAPH_H

#include "color/bcg_colors.h"
#include "renderers/bcg_attribute.h"

namespace bcg{

struct material_mesh{
    std::vector<attribute> attributes = {
            {"position", "position", 0, true},
            {"graph_color", "", 2, false}
    };

    ogl_vertex_array vao;

    bool use_uniform_color = true;
    int width;

    VectorS<3> uniform_color = color::random();
    bcg_scalar_t uniform_alpha = 1;
};

}

#endif //BCG_GRAPHICS_BCG_MATERIAL_GRAPH_H
