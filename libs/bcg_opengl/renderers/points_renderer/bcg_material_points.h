//
// Created by alex on 29.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_POINTS_H
#define BCG_GRAPHICS_BCG_MATERIAL_POINTS_H

#include "bcg_library/color/bcg_colors.h"

namespace bcg{

struct material_points{
    bool use_uniform_point_size = true;
    bool use_uniform_color = true;
    VectorS<3> uniform_color = color::random();
    bcg_scalar_t alpha = 1.0;
    std::string position_buffer;
    std::string color_buffer;
    std::string size_buffer;
};


}

#endif //BCG_GRAPHICS_BCG_MATERIAL_POINTS_H
