//
// Created by alex on 14.10.20.
//

#include "bcg_colors.h"
#include "exts/eigen/Eigen/Core"

namespace bcg{

vec3f color::random(){
    auto eigen_color = Eigen::Vector<float, 3>::Random().cwiseAbs();
    vec3f color;
    color[0] = eigen_color[0];
    color[1] = eigen_color[1];
    color[2] = eigen_color[2];
    return color;
}
vec3f color::constant(float value) {
    auto clamped = std::max(std::min(value, 1.0f), 0.0f);
    return {clamped, clamped, clamped};
}

vec3f color::rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    return {r / 255.0, g / 255.0, b / 255.0};
}

vec3f color::packed_int(unsigned int id) {
    return rgb((id & 0x000000FF) >> 0, (id & 0x0000FF00) >> 8, (id & 0x00FF0000) >> 16);
}

}