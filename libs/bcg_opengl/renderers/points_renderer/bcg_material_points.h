//
// Created by alex on 29.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_POINTS_H
#define BCG_GRAPHICS_BCG_MATERIAL_POINTS_H

#include "bcg_library/color/bcg_colors.h"
#include "renderers/bcg_attribute.h"

namespace bcg{

struct material_points{
    std::vector<attribute> attributes = {
            {"position", "v_position","v_position", 0, true},
            {"color", "", "", 1, false},
            {"point_size","", "", 2, false}
    };

    ogl_vertex_array vao;

    bool use_uniform_color = true;
    bool use_uniform_size = true;

    VectorS<3> uniform_color = color::random();
    bcg_scalar_t uniform_size = 1.0;
    bcg_scalar_t uniform_alpha = 1.0;
};


}

#endif //BCG_GRAPHICS_BCG_MATERIAL_POINTS_H
