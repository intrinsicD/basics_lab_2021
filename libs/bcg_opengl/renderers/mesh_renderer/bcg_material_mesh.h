//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_MESH_H
#define BCG_GRAPHICS_BCG_MATERIAL_MESH_H

#include "color/bcg_colors.h"
#include "renderers/bcg_attribute.h"
#include "bcg_opengl.h"

namespace bcg{

struct material_mesh{
    std::vector<attribute> attributes = {
            {"position", "v_position","v_position", 0, true},
            {"normal", "v_normal", "v_normal", 1, true},
            {"color", "", "", 2, false}
    };

    ogl_vertex_array vao;

    bool use_uniform_color = true;
    bool use_face_color = false;

    VectorS<3> ambient = color::random();
    VectorS<3> diffuse = color::white;
    VectorS<3> specular = color::grey;
    bcg_scalar_t shininess = 32;
    bcg_scalar_t uniform_alpha = 1;

    colormap::base_colormap color_map = colormap::jet();
    int width;
    ogl_texture face_colors = ogl_texture("face_color");
};

}

#endif //BCG_GRAPHICS_BCG_MATERIAL_MESH_H
