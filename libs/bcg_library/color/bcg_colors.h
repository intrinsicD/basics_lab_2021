//
// Created by alex on 14.10.20.
//

#ifndef BCG_GRAPHICS_BCG_COLORS_H
#define BCG_GRAPHICS_BCG_COLORS_H

#include <vector>
#include "math/bcg_linalg.h"

namespace bcg {

template<typename T = bcg_scalar_t>
struct color {
    [[maybe_unused]] inline static const Vector<T, 3> white = {1.0, 1.0, 1.0};
    [[maybe_unused]] inline static const Vector<T, 3> yellow = {1.0f, 1.0f, 0.0f};
    [[maybe_unused]] inline static const Vector<T, 3> magenta = {1.0f, 0.0f, 1.0f};
    [[maybe_unused]] inline static const Vector<T, 3> red = {1.0f, 0.0f, 0.0f};
    [[maybe_unused]] inline static const Vector<T, 3> cyan = {0.0f, 1.0f, 1.0f};
    [[maybe_unused]] inline static const Vector<T, 3> green = {0.0f, 1.0f, 0.0f};
    [[maybe_unused]] inline static const Vector<T, 3> blue = {0.0f, 0.0f, 1.0f};
    [[maybe_unused]] inline static const Vector<T, 3> black = {0, 0, 0};
    [[maybe_unused]] inline static const Vector<T, 3> orange = {1.0f, 0.5f, 0.0f};
    [[maybe_unused]] inline static const Vector<T, 3> violet = {0.5f, 0.0f, 1.0f};
    [[maybe_unused]] inline static const Vector<T, 3> grey = {0.5f, 0.5f, 0.5f};
    [[maybe_unused]] inline static const Vector<T, 4> default_background = {0.2f, 0.4f, 0.8f, 1.0f};

    static Vector<T, 3> random() {
        return Vector<T, 3>::Random().cwiseAbs();
    }

    static Vector<T, 3> constant(bcg_scalar_t value) {
        auto clamped = std::max<bcg_scalar_t>(std::min<bcg_scalar_t>(value, 1.0), 0.0);
        return {clamped, clamped, clamped};
    }

    static Vector<T, 3> rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
        return { r / T(255), g / T(255), b / T(255) };
    }

    static Vector<T, 3> rgba(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) {
        return { r / T(255), g / T(255), b / T(255), a / T(255) };
    }

    static Vector<T, 3> packed_int(unsigned int id) {
        return rgb((id & 0x000000FF) >> 0, (id & 0x0000FF00) >> 8, (id & 0x00FF0000) >> 16);
    }
};

}

#endif //BCG_GRAPHICS_BCG_COLORS_H
