//
// Created by Alex on 07.11.20.
//

#ifndef BCG_GRAPHICS_BCG_SEGMENT_H
#define BCG_GRAPHICS_BCG_SEGMENT_H

#include <array>
#include "math/bcg_linalg.h"

namespace bcg{

template<int N>
struct segment{
    std::array<VectorS<N>, 2> points;

    segment() : points{VectorS<N>::Zero(), VectorS<N>::Ones()}{

    }

    segment(const VectorS<N> &p0, const VectorS<N> &p1) : points{p0, p1}{

    }

    explicit segment(const std::array<VectorS<N>, 2> &points) : points(points){

    }

    void set_centered_form(const VectorS<N> &center, const VectorS<N> &direction, bcg_scalar_t extent){
        points[0] = center - extent * direction;
        points[1] = center + extent * direction;
    }

    VectorS<N> center() const{
        return (points[0] + points[1]) / 2.0;
    }

    VectorS<N> direction() const {
        return points[1] - points[0];
    }

    bcg_scalar_t extent() const {
        return direction().norm() / 2.0;
    }
};

using segment2 = segment<2>;

using segment3 = segment<3>;

}

#endif //BCG_GRAPHICS_BCG_SEGMENT_H
