//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_MESH_H
#define BCG_GRAPHICS_BCG_MATERIAL_MESH_H

#include "color/bcg_colors.h"

namespace bcg{

struct material_mesh{
    bool use_uniform_color = true;

    VectorS<3> ambient = color::random();
    VectorS<3> diffuse = color::white;
    VectorS<3> specular = color::grey;
    bcg_scalar_t shininess = 32;
    bcg_scalar_t alpha = 1;
};

}

#endif //BCG_GRAPHICS_BCG_MATERIAL_MESH_H
