//
// Created by alex on 14.10.20.
//

#include "bcg_colors.h"
#include "../exts/eigen/Eigen/Core"

namespace bcg{

VectorS<3> color::random(){
    return VectorS<3>::Random().cwiseAbs();
}

VectorS<3> color::constant(bcg_scalar_t value) {
    auto clamped = std::max<bcg_scalar_t>(std::min<bcg_scalar_t>(value, 1.0), 0.0);
    return {clamped, clamped, clamped};
}

VectorS<3> color::rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    return {r / 255.0, g / 255.0, b / 255.0};
}

VectorS<3> color::packed_int(unsigned int id) {
    return rgb((id & 0x000000FF) >> 0, (id & 0x0000FF00) >> 8, (id & 0x00FF0000) >> 16);
}

}