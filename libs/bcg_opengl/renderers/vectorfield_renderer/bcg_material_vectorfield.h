//
// Created by alex on 11.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_VECTORFIELD_H
#define BCG_GRAPHICS_BCG_MATERIAL_VECTORFIELD_H

#include <vector>
#include "bcg_opengl.h"
#include "color/bcg_colors.h"
#include "renderers/bcg_attribute.h"

namespace bcg{

struct material_vectorfield{
    std::vector<attribute> attributes = {
            {"position", "v_position","v_position", 0, true, false},
            {"vectors", "", "", 1, false, false},
            {"color", "", "", 2, false, false},
    };

    bool enabled = false;

    ogl_vertex_array vao;

    bool use_uniform_color = true;
    bool use_uniform_size = true;

    VectorS<3> uniform_color = color::random();
    bcg_scalar_t uniform_size = 1.0;
    bcg_scalar_t uniform_alpha = 1.0;
    size_t num_vectors = 0;
};

}

#endif //BCG_GRAPHICS_BCG_MATERIAL_VECTORFIELD_H
