//
// Created by alex on 14.10.20.
//

#ifndef BCG_GRAPHICS_BCG_COLORS_H
#define BCG_GRAPHICS_BCG_COLORS_H

#include <vector>
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

namespace colormap {

struct base_colormap {
    base_colormap() = default;

    base_colormap(std::vector<VectorS < 3>>

    colorpath) :
    colorpath(colorpath){}

    std::vector<VectorS < 3>> colorpath;

    std::vector<VectorS < 3>> operator()(
    const std::vector<bcg_scalar_t> &scalarfield, bcg_scalar_t
    minClamp,
    bcg_scalar_t maxClamp
    ) const;

    std::vector<VectorS < 3>> operator()(
    const VectorS<-1> &scalarfield,
            bcg_scalar_t
    minClamp,
    bcg_scalar_t maxClamp
    ) const;

    VectorS<3> interpolate(bcg_scalar_t t, const VectorS<3> &from, const VectorS<3> &to) const;

    bcg_scalar_t quantize(bcg_scalar_t t, size_t num_bins) const;

    size_t interval(bcg_scalar_t t) const;
};

struct grey : public base_colormap {
    grey() : base_colormap({color::black,
                            color::white}) {

    }
};

struct hot : public base_colormap {
    hot() : base_colormap({color::black,
                           color::red,
                           color::yellow,
                           color::white}) {

    }
};


struct temperature : public base_colormap {
    temperature() : base_colormap({color::black,
                                   color::blue,
                                   color::green,
                                   color::red,
                                   color::yellow,
                                   color::white}) {

    }
};

struct rainbow : public base_colormap {
    rainbow() : base_colormap({color::red,
                               color::orange,
                               color::yellow,
                               color::green,
                               color::cyan,
                               color::blue,
                               color::violet}) {

    }
};

struct jet : public base_colormap {
    jet() : base_colormap({color::blue,
                           color::cyan,
                           color::green,
                           color::yellow,
                           color::orange,
                           color::red}) {

    }
};

struct vidris : public base_colormap {
    vidris() : base_colormap({color::violet,
                              color::blue,
                              color::green,
                              color::yellow}) {

    }
};

struct hsv : public base_colormap {
    hsv() : base_colormap({color::red,
                           color::orange,
                           color::yellow,
                           color::green,
                           color::cyan,
                           color::blue,
                           color::violet,
                           color::magenta,
                           color::red}) {

    }
};

struct coolwarm : public base_colormap {
    coolwarm() : base_colormap({color::blue,
                                color::white,
                                color::red}) {

    }
};

struct colormap_selector {
    enum colormap_type {
        GREY,
        HOT,
        TEMPERATURE,
        RAINBOW,
        JET,
        VIDRIS,
        HSV,
        COOLWARM,
        __LAST__
    };

    static std::vector<std::string> get_names();

    static base_colormap get_colormap(colormap_type type);
};

}

}

#endif //BCG_GRAPHICS_BCG_COLORS_H
