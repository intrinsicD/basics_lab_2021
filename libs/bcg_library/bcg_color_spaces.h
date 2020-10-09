//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_COLOR_SPACES_H
#define BCG_GRAPHICS_BCG_COLOR_SPACES_H

#include "bcg_vector.h"

namespace bcg{

// RGB color spaces
enum struct color_space {
    rgb,         // default linear space (srgb linear)
    srgb,        // srgb color space (non-linear)
    adobe,       // Adobe rgb color space (non-linear)
    prophoto,    // ProPhoto Kodak rgb color space (non-linear)
    rec709,      // hdtv color space (non-linear)
    rec2020,     // uhtv color space (non-linear)
    rec2100pq,   // hdr color space with perceptual quantizer (non-linear)
    rec2100hlg,  // hdr color space with hybrid log gamma (non-linear)
    aces2065,    // ACES storage format (linear)
    acescg,      // ACES CG computation (linear)
    acescc,      // ACES color correction (non-linear)
    acescct,     // ACES color correction 2 (non-linear)
    p3dci,       // P3 DCI (non-linear)
    p3d60,       // P3 variation for D60 (non-linear)
    p3d65,       // P3 variation for D65 (non-linear)
    p3display,   // Apple display P3
};

// Conversion between rgb color spaces
vec3f color_to_xyz(const vec3f& col, color_space from);
vec3f xyz_to_color(const vec3f& xyz, color_space to);

}

#endif //BCG_GRAPHICS_BCG_COLOR_SPACES_H
