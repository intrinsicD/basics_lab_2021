//
// Created by alex on 25.03.21.
//

#ifndef BCG_GRAPHICS_BCG_COLOR_RGB_H
#define BCG_GRAPHICS_BCG_COLOR_RGB_H

#include "math/vector/bcg_vector.h"

namespace bcg{

Vector<int, 3> hex2rgb(int hex_color);

int rgb2hex(int r, int g, int b);

int rgb2hex(const Vector<int, 3> &rgb);

}

#endif //BCG_GRAPHICS_BCG_COLOR_RGB_H
