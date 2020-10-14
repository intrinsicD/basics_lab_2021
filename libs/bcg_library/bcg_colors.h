//
// Created by alex on 14.10.20.
//

#ifndef BCG_GRAPHICS_BCG_COLORS_H
#define BCG_GRAPHICS_BCG_COLORS_H

#include "math/bcg_linalg.h"

namespace bcg {

struct color {
    [[maybe_unused]] inline static constexpr auto white = one3f;
    [[maybe_unused]] inline static constexpr auto yellow = {1.0f, 1.0f, 0.0f};
    [[maybe_unused]] inline static constexpr auto magenta = {1.0f, 0.0f, 1.0f};
    [[maybe_unused]] inline static constexpr auto red = {1.0f, 0.0f, 0.0f};
    [[maybe_unused]] inline static constexpr auto cyan = {0.0f, 1.0f, 1.0f};
    [[maybe_unused]] inline static constexpr auto green = {0.0f, 1.0f, 0.0f};
    [[maybe_unused]] inline static constexpr auto blue = {0.0f, 0.0f, 1.0f};
    [[maybe_unused]] inline static constexpr auto black = zero3f;
    [[maybe_unused]] inline static constexpr auto orange = {1.0f, 0.5f, 0.0f};
    [[maybe_unused]] inline static constexpr auto violet = {0.5f, 0.0f, 1.0f};
    [[maybe_unused]] inline static constexpr auto grey = {0.5f, 0.5f, 0.5f};

    static vec3f random();

    static vec3f constant(float value);

    static vec3f rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b);

    static vec3f packed_int(unsigned int id);
};

}

#endif //BCG_GRAPHICS_BCG_COLORS_H
