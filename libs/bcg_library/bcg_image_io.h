//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_IMAGE_IO_H
#define BCG_GRAPHICS_BCG_IMAGE_IO_H

// -----------------------------------------------------------------------------
// INCLUDES
// -----------------------------------------------------------------------------

#include "bcg_image.h"

namespace bcg {

// Check if an image is HDR based on filename.
bool is_hdr_filename(const std::string &filename);

// Loads/saves a 4 channels float/byte image in linear/srgb color space.
bool load_image(const std::string &filename, image<vec4f> &img, std::string &error);

bool save_image(const std::string &filename, const image<vec4f> &img, std::string &error);

bool load_image(const std::string &filename, image<vec4b> &img, std::string &error);

bool save_image(const std::string &filename, const image<vec4b> &img, std::string &error);

// Load/saves a 16 bit image in linear color space.
bool load_image(const std::string &filename, image<vec4s> &img, std::string &error);

bool save_image(const std::string &filename, const image<vec4s> &img, std::string &error);

// Loads/saves a  channel float/byte image in linear/srgb color space.
bool load_image(const std::string &filename, image<vec4f> &imgf, image<vec4b> &imgb,
                std::string &error);

bool save_image(const std::string &filename, const image<vec4f> &imgf,
                const image<vec4b> &imgb, std::string &error);
}

#endif //BCG_GRAPHICS_BCG_IMAGE_IO_H
