//
// Created by alex on 09.10.20.
//

#include <stdexcept>
#include "bcg_color_spaces.h"
#include "bcg_matrix.h"
#include "bcg_math_constant.h"

namespace bcg {

struct color_space_params {
    // Curve type
    enum struct curve_t {
        linear,
        gamma,
        linear_gamma,
        aces_cc,
        aces_cct,
        pq,
        hlg
    };
    // primaries
    vec2f red_chromaticity;    // xy chromaticity of the red primary
    vec2f green_chromaticity;  // xy chromaticity of the green primary
    vec2f blue_chromaticity;   // xy chromaticity of the blue primary
    vec2f white_chromaticity;  // xy chromaticity of the white point
    mat3f rgb_to_xyz_mat;      // matrix from rgb to xyz
    mat3f xyz_to_rgb_mat;      // matrix from xyz to rgb
    // tone curve
    curve_t curve_type;
    float curve_gamma;  // gamma for power curves
    vec4f curve_abcd;   // tone curve values for linear_gamma curves
};

inline mat3f rgb_to_xyz_mat(const vec2f &rc, const vec2f &gc, const vec2f &bc, const vec2f &wc) {
    auto rgb = mat3f{
            {rc.x, rc.y, 1 - rc.x - rc.y},
            {gc.x, gc.y, 1 - gc.x - gc.y},
            {bc.x, bc.y, 1 - bc.x - bc.y},
    };
    auto w = vec3f{wc.x, wc.y, 1 - wc.x - wc.y};
    auto c = inverse(rgb) * vec3f{w.x / w.y, 1, w.z / w.y};
    return mat3f{c.x * rgb.x, c.y * rgb.y, c.z * rgb.z};
}

// Construct an RGB color space. Predefined color spaces below
inline color_space_params get_color_scape_params(color_space space) {
    static auto make_linear_rgb_space = [](const vec2f &red, const vec2f &green,
                                           const vec2f &blue,
                                           const vec2f &white) {
        return color_space_params{red, green, blue, white,
                                  rgb_to_xyz_mat(red, green, blue, white),
                                  inverse(rgb_to_xyz_mat(red, green, blue, white)),
                                  color_space_params::curve_t::linear};
    };
    static auto make_gamma_rgb_space =
            [](const vec2f &red, const vec2f &green, const vec2f &blue,
               const vec2f &white, float gamma, const vec4f &curve_abcd = zero4f) {
                return color_space_params{red, green, blue, white,
                                          rgb_to_xyz_mat(red, green, blue, white),
                                          inverse(rgb_to_xyz_mat(red, green, blue, white)),
                                          curve_abcd == zero4f ? color_space_params::curve_t::gamma
                                                               : color_space_params::curve_t::linear_gamma};
            };
    static auto make_other_rgb_space =
            [](const vec2f &red, const vec2f &green, const vec2f &blue,
               const vec2f &white, color_space_params::curve_t curve_type) {
                return color_space_params{red, green, blue, white,
                                          rgb_to_xyz_mat(red, green, blue, white),
                                          inverse(rgb_to_xyz_mat(red, green, blue, white)), curve_type};
            };

    // color space parameters
    // https://en.wikipedia.org/wiki/Rec._709
    static auto rgb_params = make_linear_rgb_space(
            {0.6400, 0.3300}, {0.3000, 0.6000}, {0.1500, 0.0600}, {0.3127, 0.3290});
    // https://en.wikipedia.org/wiki/Rec._709
    static auto srgb_params = make_gamma_rgb_space({0.6400, 0.3300},
                                                   {0.3000, 0.6000}, {0.1500, 0.0600}, {0.3127, 0.3290}, 2.4,
                                                   {1.055, 0.055, 12.92, 0.0031308});
    // https://en.wikipedia.org/wiki/Academy_Color_Encoding_System
    static auto aces2065_params = make_linear_rgb_space({0.7347, 0.2653},
                                                        {0.0000, 1.0000}, {0.0001, -0.0770}, {0.32168, 0.33767});
    // https://en.wikipedia.org/wiki/Academy_Color_Encoding_Systemx
    static auto acescg_params = make_linear_rgb_space({0.7130, 0.2930},
                                                      {0.1650, 0.8300}, {0.1280, +0.0440}, {0.32168, 0.33767});
    // https://en.wikipedia.org/wiki/Academy_Color_Encoding_Systemx
    static auto acescc_params = make_other_rgb_space({0.7130, 0.2930},
                                                     {0.1650, 0.8300}, {0.1280, +0.0440}, {0.32168, 0.33767},
                                                     color_space_params::curve_t::aces_cc);
    // https://en.wikipedia.org/wiki/Academy_Color_Encoding_Systemx
    static auto acescct_params = make_other_rgb_space({0.7130, 0.2930},
                                                      {0.1650, 0.8300}, {0.1280, +0.0440}, {0.32168, 0.33767},
                                                      color_space_params::curve_t::aces_cct);
    // https://en.wikipedia.org/wiki/Adobe_RGB_color_space
    static auto adobe_params = make_gamma_rgb_space({0.6400, 0.3300},
                                                    {0.2100, 0.7100}, {0.1500, 0.0600}, {0.3127, 0.3290}, 2.19921875);
    // https://en.wikipedia.org/wiki/Rec._709
    static auto rec709_params = make_gamma_rgb_space({0.6400, 0.3300},
                                                     {0.3000, 0.6000}, {0.1500, 0.0600}, {0.3127, 0.3290}, 1 / 0.45,
                                                     {1.099, 0.099, 4.500, 0.018});
    // https://en.wikipedia.org/wiki/Rec._2020
    static auto rec2020_params = make_gamma_rgb_space({0.7080, 0.2920},
                                                      {0.1700, 0.7970}, {0.1310, 0.0460}, {0.3127, 0.3290}, 1 / 0.45,
                                                      {1.09929682680944, 0.09929682680944, 4.5, 0.018053968510807});
    // https://en.wikipedia.org/wiki/Rec._2020
    static auto rec2100pq_params = make_other_rgb_space({0.7080, 0.2920},
                                                        {0.1700, 0.7970}, {0.1310, 0.0460}, {0.3127, 0.3290},
                                                        color_space_params::curve_t::pq);
    // https://en.wikipedia.org/wiki/Rec._2020
    static auto rec2100hlg_params = make_other_rgb_space({0.7080, 0.2920},
                                                         {0.1700, 0.7970}, {0.1310, 0.0460}, {0.3127, 0.3290},
                                                         color_space_params::curve_t::hlg);
    // https://en.wikipedia.org/wiki/DCI-P3
    static auto p3dci_params = make_gamma_rgb_space({0.6800, 0.3200},
                                                    {0.2650, 0.6900}, {0.1500, 0.0600}, {0.3140, 0.3510}, 1.6);
    // https://en.wikipedia.org/wiki/DCI-P3
    static auto p3d60_params = make_gamma_rgb_space({0.6800, 0.3200},
                                                    {0.2650, 0.6900}, {0.1500, 0.0600}, {0.32168, 0.33767}, 1.6);
    // https://en.wikipedia.org/wiki/DCI-P3
    static auto p3d65_params = make_gamma_rgb_space({0.6800, 0.3200},
                                                    {0.2650, 0.6900}, {0.1500, 0.0600}, {0.3127, 0.3290}, 1.6);
    // https://en.wikipedia.org/wiki/DCI-P3
    static auto p3display_params = make_gamma_rgb_space({0.6800, 0.3200},
                                                        {0.2650, 0.6900}, {0.1500, 0.0600}, {0.3127, 0.3290}, 2.4,
                                                        {1.055, 0.055, 12.92, 0.0031308});
    // https://en.wikipedia.org/wiki/ProPhoto_RGB_color_space
    static auto prophoto_params = make_gamma_rgb_space({0.7347, 0.2653},
                                                       {0.1596, 0.8404}, {0.0366, 0.0001}, {0.3457, 0.3585}, 1.8,
                                                       {1, 0, 16, 0.001953125});

    // return values;
    switch (space) {
        case color_space::rgb:
            return rgb_params;
        case color_space::srgb:
            return srgb_params;
        case color_space::adobe:
            return adobe_params;
        case color_space::prophoto:
            return prophoto_params;
        case color_space::rec709:
            return rec709_params;
        case color_space::rec2020:
            return rec2020_params;
        case color_space::rec2100pq:
            return rec2100pq_params;
        case color_space::rec2100hlg:
            return rec2100hlg_params;
        case color_space::aces2065:
            return aces2065_params;
        case color_space::acescg:
            return acescg_params;
        case color_space::acescc:
            return acescc_params;
        case color_space::acescct:
            return acescct_params;
        case color_space::p3dci:
            return p3dci_params;
        case color_space::p3d60:
            return p3d60_params;
        case color_space::p3d65:
            return p3d65_params;
        case color_space::p3display:
            return p3display_params;
        default:
            throw std::runtime_error{"should not have gotten here"};
    }

    // return here to silence warnings
    throw std::runtime_error{"should not have gotten here"};
    return {};
}

// gamma to linear
inline float gamma_display_to_linear(float x, float gamma) {
    return pow(x, gamma);
}

inline float gamma_linear_to_display(float x, float gamma) {
    return pow(x, 1 / gamma);
}

// https://en.wikipedia.org/wiki/Rec._709
inline float gamma_display_to_linear(float x, float gamma, const vec4f &abcd) {
    auto&[a, b, c, d] = abcd;
    if (x < 1 / d) {
        return x / c;
    } else {
        return pow((x + b) / a, gamma);
    }
}

inline float gamma_linear_to_display(float x, float gamma, const vec4f &abcd) {
    auto&[a, b, c, d] = abcd;
    if (x < d) {
        return x * c;
    } else {
        return a * pow(x, 1 / gamma) - b;
    }
}

// https://en.wikipedia.org/wiki/Academy_Color_Encoding_Systemx
inline float acescc_display_to_linear(float x) {
    if (x < -0.3013698630f) {  // (9.72-15)/17.52
        return (exp2(x * 17.52f - 9.72f) - exp2(-16.0f)) * 2;
    } else if (x < (log2(65504.0f) + 9.72f) / 17.52f) {
        return exp2(x * 17.52f - 9.72f);
    } else {  // (in >= (log2(65504)+9.72)/17.52)
        return 65504.0f;
    }
}

// https://en.wikipedia.org/wiki/Academy_Color_Encoding_Systemx
inline float acescct_display_to_linear(float x) {
    if (x < 0.155251141552511f) {
        return (x - 0.0729055341958355f) / 10.5402377416545f;
    } else {
        return exp2(x * 17.52f - 9.72f);
    }
}

// https://en.wikipedia.org/wiki/Academy_Color_Encoding_Systemx
inline float acescc_linear_to_display(float x) {
    if (x <= 0) {
        return -0.3584474886f;  // =(log2( pow(2.,-16.))+9.72)/17.52
    } else if (x < exp2(-15.0f)) {
        return (log2(exp2(-16.0f) + x * 0.5f) + 9.72f) / 17.52f;
    } else {  // (in >= pow(2.,-15))
        return (log2(x) + 9.72f) / 17.52f;
    }
}

// https://en.wikipedia.org/wiki/Academy_Color_Encoding_Systemx
inline float acescct_linear_to_display(float x) {
    if (x <= 0.0078125f) {
        return 10.5402377416545f * x + 0.0729055341958355f;
    } else {
        return (log2(x) + 9.72f) / 17.52f;
    }
}

// https://en.wikipedia.org/wiki/High-dynamic-range_video#Perceptual_Quantizer
// https://github.com/ampas/aces-dev/blob/master/transforms/ctl/lib/ACESlib.Utilities_Color.ctl
// In PQ, we assume that the linear luminance in [0,1] corresponds to
// [0,10000] cd m^2
inline float pq_display_to_linear(float x) {
    auto Np = pow(x, 1 / 78.84375f);
    auto L = max(Np - 0.8359375f, 0.0f);
    L = L / (18.8515625f - 18.6875f * Np);
    L = pow(L, 1 / 0.1593017578125f);
    return L;
}

inline float pq_linear_to_display(float x) {
    return pow((0.8359375f + 18.8515625f * pow(x, 0.1593017578125f)) /
               (1 + 18.6875f * pow(x, 0.1593017578125f)),
               78.84375f);
}

// https://en.wikipedia.org/wiki/High-dynamic-range_video#Perceptual_Quantizer
// In HLG, we assume that the linear luminance in [0,1] corresponds to
// [0,1000] cd m^2. Note that the version we report here is scaled in [0,1]
// range for nominal luminance. But HLG was initially defined in the [0,12]
// range where it maps 1 to 0.5 and 12 to 1. For use in HDR tonemapping that is
// likely a better range to use.
inline float hlg_display_to_linear(float x) {
    if (x < 0.5f) {
        return 3 * 3 * x * x;
    } else {
        return (exp((x - 0.55991073f) / 0.17883277f) + 0.28466892f) / 12;
    }
}

inline float hlg_linear_to_display(float x) {
    if (x < 1 / 12.0f) {
        return sqrt(3 * x);
    } else {
        return 0.17883277f * log(12 * x - 0.28466892f) + 0.55991073f;
    }
}

// Conversion to/from xyz
vec3f color_to_xyz(const vec3f &col, color_space from) {
    auto space = get_color_scape_params(from);
    auto rgb = col;
    if (space.curve_type == color_space_params::curve_t::linear) {
        // do nothing
    } else if (space.curve_type == color_space_params::curve_t::gamma) {
        rgb = {
                gamma_linear_to_display(rgb.x, space.curve_gamma),
                gamma_linear_to_display(rgb.y, space.curve_gamma),
                gamma_linear_to_display(rgb.z, space.curve_gamma),
        };
    } else if (space.curve_type == color_space_params::curve_t::linear_gamma) {
        rgb = {
                gamma_linear_to_display(rgb.x, space.curve_gamma, space.curve_abcd),
                gamma_linear_to_display(rgb.y, space.curve_gamma, space.curve_abcd),
                gamma_linear_to_display(rgb.z, space.curve_gamma, space.curve_abcd),
        };
    } else if (space.curve_type == color_space_params::curve_t::aces_cc) {
        rgb = {
                acescc_linear_to_display(rgb.x),
                acescc_linear_to_display(rgb.y),
                acescc_linear_to_display(rgb.z),
        };
    } else if (space.curve_type == color_space_params::curve_t::aces_cct) {
        rgb = {
                acescct_linear_to_display(rgb.x),
                acescct_linear_to_display(rgb.y),
                acescct_linear_to_display(rgb.z),
        };
    } else if (space.curve_type == color_space_params::curve_t::pq) {
        rgb = {
                pq_linear_to_display(rgb.x),
                pq_linear_to_display(rgb.y),
                pq_linear_to_display(rgb.z),
        };
    } else if (space.curve_type == color_space_params::curve_t::hlg) {
        rgb = {
                hlg_linear_to_display(rgb.x),
                hlg_linear_to_display(rgb.y),
                hlg_linear_to_display(rgb.z),
        };
    } else {
        throw std::runtime_error{"should not have gotten here"};
    }
    return space.rgb_to_xyz_mat * rgb;
}

vec3f xyz_to_color(const vec3f &xyz, color_space to) {
    auto space = get_color_scape_params(to);
    auto rgb = space.xyz_to_rgb_mat * xyz;
    if (space.curve_type == color_space_params::curve_t::linear) {
        // nothing
    } else if (space.curve_type == color_space_params::curve_t::gamma) {
        rgb = {
                gamma_display_to_linear(rgb.x, space.curve_gamma),
                gamma_display_to_linear(rgb.y, space.curve_gamma),
                gamma_display_to_linear(rgb.z, space.curve_gamma),
        };
    } else if (space.curve_type == color_space_params::curve_t::linear_gamma) {
        rgb = {
                gamma_display_to_linear(rgb.x, space.curve_gamma, space.curve_abcd),
                gamma_display_to_linear(rgb.y, space.curve_gamma, space.curve_abcd),
                gamma_display_to_linear(rgb.z, space.curve_gamma, space.curve_abcd),
        };
    } else if (space.curve_type == color_space_params::curve_t::aces_cc) {
        rgb = {
                acescc_display_to_linear(rgb.x),
                acescc_display_to_linear(rgb.y),
                acescc_display_to_linear(rgb.z),
        };
    } else if (space.curve_type == color_space_params::curve_t::aces_cct) {
        rgb = {
                acescct_display_to_linear(rgb.x),
                acescct_display_to_linear(rgb.y),
                acescct_display_to_linear(rgb.z),
        };
    } else if (space.curve_type == color_space_params::curve_t::pq) {
        rgb = {
                pq_display_to_linear(rgb.x),
                pq_display_to_linear(rgb.y),
                pq_display_to_linear(rgb.z),
        };
    } else if (space.curve_type == color_space_params::curve_t::hlg) {
        rgb = {
                hlg_display_to_linear(rgb.x),
                hlg_display_to_linear(rgb.y),
                hlg_display_to_linear(rgb.z),
        };
    } else {
        throw std::runtime_error{"should not have gotten here"};
    }
    return rgb;
}

vec3f convert_color(const vec3f &col, color_space from, color_space to) {
    if (from == to) return col;
    return xyz_to_color(color_to_xyz(col, from), to);
}

}