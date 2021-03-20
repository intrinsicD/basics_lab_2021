//
// Created by alex on 22.02.21.
//

#include "bcg_colormap.h"
#include "math/vector/bcg_vector_map_eigen.h"

namespace bcg {

namespace colormap {

std::vector<VectorS<3>> base_colormap::operator()(const VectorS<-1> &scalarfield,
                                                  bcg_scalar_t minClamp,
                                                  bcg_scalar_t maxClamp) const {
    std::vector<VectorS<3>> colors;
    auto min = scalarfield.minCoeff();
    auto max = scalarfield.maxCoeff();

    if (minClamp == maxClamp && minClamp == -1) {
        minClamp = min;
        maxClamp = max;
    }
    auto c_min = std::max(minClamp, min);
    auto c_max = std::min(maxClamp, max);
    auto range = c_max - c_min;

    if (range == 0) {
        c_min = 0;
        range = 1;
    }
    for (auto i = 0; i < scalarfield.size(); ++i) {
        auto s = scalarfield[i];
        if (std::isnan(s)) {
            s = c_min;
        }
        auto t = std::min<bcg_scalar_t>(std::max<bcg_scalar_t>((s - c_min) / range, 0.0), 1.0);
        size_t j = interval(t);

        t = t * (colorpath.size() - 1.0) - j;

        colors.push_back(interpolate(t, colorpath[j], colorpath[j + 1]));
    }
    return colors;
}

std::vector<VectorS<3>> base_colormap::operator()(const std::vector<bcg_scalar_t> &scalarfield,
                                                  bcg_scalar_t minClamp,
                                                  bcg_scalar_t maxClamp) const {
    return (*this)(MapConst(scalarfield), minClamp, maxClamp);
}

VectorS<3> base_colormap::interpolate(bcg_scalar_t t, const VectorS<3> &from, const VectorS<3> &to) const {
    return t * to + (1.0 - t) * from;
}

bcg_scalar_t base_colormap::quantize(bcg_scalar_t t, size_t num_bins) const {
    return std::floor(t * num_bins) / bcg_scalar_t(num_bins - 1.0);
}

size_t base_colormap::interval(bcg_scalar_t t) const {
    size_t n = colorpath.size() - 1;
    auto i = static_cast<size_t>(std::floor(t * n));
    assert(colorpath.size() > 1);
    return (i == n && i > 0) ? i - 1 : i;
}

std::vector<std::string> colormap_selector::get_names() {
    std::vector<std::string> names(colormap_type::__LAST__);
    names[GREY] = "GREY";
    names[HOT] = "HOT";
    names[TEMPERATURE] = "TEMPERATURE";
    names[RAINBOW] = "RAINBOW";
    names[JET] = "JET";
    names[VIDRIS] = "VIDRIS";
    names[HSV] = "HSV";
    names[COOLWARM] = "COOLWARM";
    return names;
}

base_colormap colormap_selector::get_colormap(colormap_type type) {
    switch (type) {
        case GREY : {
            return grey();
        }
        case HOT : {
            return hot();
        }
        case TEMPERATURE : {
            return temperature();
        }
        case RAINBOW : {
            return rainbow();
        }
        case JET : {
            return jet();
        }
        case VIDRIS : {
            return vidris();
        }
        case HSV : {
            return hsv();
        }
        case COOLWARM : {
            return coolwarm();
        }
        //case __LAST__ : {
        default : {
            return base_colormap();
        }
    }
}

}

}