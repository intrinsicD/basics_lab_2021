//
// Created by alex on 14.10.20.
//

#ifndef BCG_GRAPHICS_BCG_COLORS_H
#define BCG_GRAPHICS_BCG_COLORS_H

#include "math/bcg_linalg.h"

namespace bcg {

struct color {
    [[maybe_unused]] inline static const auto white = one3s;
    [[maybe_unused]] inline static const auto yellow = {1.0f, 1.0f, 0.0f};
    [[maybe_unused]] inline static const auto magenta = {1.0f, 0.0f, 1.0f};
    [[maybe_unused]] inline static const auto red = {1.0f, 0.0f, 0.0f};
    [[maybe_unused]] inline static const auto cyan = {0.0f, 1.0f, 1.0f};
    [[maybe_unused]] inline static const auto green = {0.0f, 1.0f, 0.0f};
    [[maybe_unused]] inline static const auto blue = {0.0f, 0.0f, 1.0f};
    [[maybe_unused]] inline static const auto black = zero3s;
    [[maybe_unused]] inline static const auto orange = {1.0f, 0.5f, 0.0f};
    [[maybe_unused]] inline static const auto violet = {0.5f, 0.0f, 1.0f};
    [[maybe_unused]] inline static const auto grey = {0.5f, 0.5f, 0.5f};

    static VectorS<3> random();

    static VectorS<3> constant(bcg_scalar_t value);

    static VectorS<3> rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b);

    static VectorS<3> packed_int(unsigned int id);
};

}

#endif //BCG_GRAPHICS_BCG_COLORS_H
