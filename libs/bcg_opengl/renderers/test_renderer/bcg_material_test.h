//
// Created by alex on 29.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MATERIAL_POINTS_H
#define BCG_GRAPHICS_BCG_MATERIAL_POINTS_H

#include "bcg_library/color/bcg_colors.h"

namespace bcg{

struct material_test{
    VectorS<4> uniform_color = color::red.homogeneous();
};


}

#endif //BCG_GRAPHICS_BCG_MATERIAL_POINTS_H
