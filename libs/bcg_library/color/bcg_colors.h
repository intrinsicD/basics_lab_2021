//
// Created by alex on 14.10.20.
//

#ifndef BCG_GRAPHICS_BCG_COLORS_H
#define BCG_GRAPHICS_BCG_COLORS_H

#include <vector>
#include "math/bcg_linalg.h"
#include "utils/bcg_macros.h"

namespace bcg {

template<typename T = bcg_scalar_t>
struct color {
    BCG_MAYBE_UNUSED inline static const Vector<T, 3> white = Vector<T, 3>(1.0, 1.0, 1.0);
    BCG_MAYBE_UNUSED inline static const Vector<T, 3> yellow = Vector<T, 3>(1.0, 1.0, 0.0);
    BCG_MAYBE_UNUSED inline static const Vector<T, 3> magenta = Vector<T, 3>(1.0, 0.0, 1.0);
    BCG_MAYBE_UNUSED inline static const Vector<T, 3> red = Vector<T, 3>(1.0, 0.0, 0.0);
    BCG_MAYBE_UNUSED inline static const Vector<T, 3> cyan = Vector<T, 3>(0.0, 1.0, 1.0);
    BCG_MAYBE_UNUSED inline static const Vector<T, 3> green = Vector<T, 3>(0.0, 1.0, 0.0);
    BCG_MAYBE_UNUSED inline static const Vector<T, 3> blue = Vector<T, 3>(0.0, 0.0, 1.0);
    BCG_MAYBE_UNUSED inline static const Vector<T, 3> black = Vector<T, 3>(0.0, 0.0, 0.0);
    BCG_MAYBE_UNUSED inline static const Vector<T, 3> orange = Vector<T, 3>(1.0, 0.5, 0.0);
    BCG_MAYBE_UNUSED inline static const Vector<T, 3> violet = Vector<T, 3>(0.5, 0.0, 1.0);
    BCG_MAYBE_UNUSED inline static const Vector<T, 3> grey = Vector<T, 3>(0.5, 0.5, 0.5);
    BCG_MAYBE_UNUSED inline static const Vector<T, 4> default_background = Vector<T, 4>(0.2, 0.4, 0.8, 1.0);

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
