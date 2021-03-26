//
// Created by alex on 25.03.21.
//

#include "bcg_color_rgb.h"

namespace bcg{

Vector<int, 3> hex2rgb(int hex_color){
    return {( hex_color >> 16 ) & 0xFF, ( hex_color >> 8 ) & 0xFF, hex_color & 0xFF};
}

int rgb2hex(int r, int g, int b){
    return (r << 16) + (g << 8) + b;
}

int rgb2hex(const Vector<int, 3> &rgb){
    return rgb2hex(rgb[0], rgb[1], rgb[2]);
}

}