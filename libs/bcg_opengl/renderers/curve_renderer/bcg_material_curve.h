//
// Created by alex on 13.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_CURVE_H
#define BCG_GRAPHICS_BCG_MATERIAL_CURVE_H

#include "bcg_opengl.h"
#include "color/bcg_colors.h"
#include "renderers/bcg_attribute.h"

namespace bcg{

struct material_curve{
    std::vector<attribute> attributes = {
            {"position", "v_position", "v_position", 0, true},
    };

    ogl_vertex_array vao;

    bool show_bezier = true, show_hermite = false;

    VectorS<3> uniform_color = color::random();
    bcg_scalar_t uniform_alpha = 1;
    int tesselation_level = 16;
    int width;
    ogl_texture control_points = ogl_texture("control_points");
};

}

#endif //BCG_GRAPHICS_BCG_MATERIAL_CURVE_H
