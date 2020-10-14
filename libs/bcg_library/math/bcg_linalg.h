//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_LINALG_H
#define BCG_GRAPHICS_BCG_LINALG_H

// -----------------------------------------------------------------------------
// INCLUDES
// -----------------------------------------------------------------------------

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/component_wise.hpp"
#include "glm/gtx/string_cast.hpp"

using namespace glm;

using byte = unsigned char;
using uint = unsigned int;
using ushort = uint16_t;

using mat2f = glm::fmat2;
using mat2d = glm::dmat2;

using mat3f = glm::fmat3;
using mat3d = glm::dmat3;

using mat4f = glm::fmat4;
using mat4d = glm::dmat4;

using vec2i = glm::ivec2;
using vec2f = glm::fvec2;
using vec2d = glm::dvec2;

using vec3i = glm::ivec3;
using vec3f = glm::fvec3;
using vec3d = glm::dvec3;

using vec4i = glm::ivec4;
using vec4f = glm::fvec4;
using vec4d = glm::dvec4;

template<int L, typename T, qualifier Q>
inline std::ostream &operator<<(std::ostream &stream, const glm::vec<L, T, Q> &vec){
    stream << to_string(vec) << "\n";
    return stream;
}

// Zero vector constants.
[[maybe_unused]] inline constexpr auto zero2i = vec2i{0, 0};
[[maybe_unused]] inline constexpr auto zero3i = vec3i{0, 0, 0};
[[maybe_unused]] inline const auto zero4i = vec4i{0, 0, 0, 0};

// One vector constants.
[[maybe_unused]] inline constexpr auto one2i = vec2i{1, 1};
[[maybe_unused]] inline constexpr auto one3i = vec3i{1, 1, 1};
[[maybe_unused]] inline const auto one4i = vec4i{1, 1, 1, 1};


// Zero vector constants.
[[maybe_unused]] inline constexpr auto zero2f = vec2f{0, 0};
[[maybe_unused]] inline constexpr auto zero3f = vec3f{0, 0, 0};
[[maybe_unused]] inline const auto zero4f = vec4f{0, 0, 0, 0};

// One vector constants.
[[maybe_unused]] inline constexpr auto one2f = vec2f{1, 1};
[[maybe_unused]] inline constexpr auto one3f = vec3f{1, 1, 1};
[[maybe_unused]] inline const auto one4f = vec4f{1, 1, 1, 1};

// Zero vector constants.
[[maybe_unused]] inline constexpr auto zero2d = vec2d{0, 0};
[[maybe_unused]] inline constexpr auto zero3d = vec3d{0, 0, 0};
[[maybe_unused]] inline const auto zero4d = vec4d{0, 0, 0, 0};

// One vector constants.
[[maybe_unused]] inline constexpr auto one2d = vec2d{1, 1};
[[maybe_unused]] inline constexpr auto one3d = vec3d{1, 1, 1};
[[maybe_unused]] inline const auto one4d = vec4d{1, 1, 1, 1};




#endif //BCG_GRAPHICS_BCG_LINALG_H
