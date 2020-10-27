//
// Created by alex on 14.10.20.
//

#ifndef BCG_GRAPHICS_BCG_COLORS_H
#define BCG_GRAPHICS_BCG_COLORS_H

#include "math/bcg_linalg.h"

namespace bcg {

struct color {
    [[maybe_unused]] inline static const VectorS<3> white = one3s;
    [[maybe_unused]] inline static const VectorS<3> yellow = {1.0f, 1.0f, 0.0f};
    [[maybe_unused]] inline static const VectorS<3> magenta = {1.0f, 0.0f, 1.0f};
    [[maybe_unused]] inline static const VectorS<3> red = {1.0f, 0.0f, 0.0f};
    [[maybe_unused]] inline static const VectorS<3> cyan = {0.0f, 1.0f, 1.0f};
    [[maybe_unused]] inline static const VectorS<3> green = {0.0f, 1.0f, 0.0f};
    [[maybe_unused]] inline static const VectorS<3> blue = {0.0f, 0.0f, 1.0f};
    [[maybe_unused]] inline static const VectorS<3> black = zero3s;
    [[maybe_unused]] inline static const VectorS<3> orange = {1.0f, 0.5f, 0.0f};
    [[maybe_unused]] inline static const VectorS<3> violet = {0.5f, 0.0f, 1.0f};
    [[maybe_unused]] inline static const VectorS<3> grey = {0.5f, 0.5f, 0.5f};
    [[maybe_unused]] inline static const VectorS<4> default_background = {0.2f, 0.4f, 0.8f, 1.0f};

    static VectorS<3> random();

    static VectorS<3> constant(bcg_scalar_t value);

    static VectorS<3> rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b);

    static VectorS<3> packed_int(unsigned int id);
};

}

#endif //BCG_GRAPHICS_BCG_COLORS_H
