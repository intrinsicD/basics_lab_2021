//
// Created by alex on 13.11.20.
//

#ifndef BCG_GRAPHICS_BCG_CURVE_CIRCLE_H
#define BCG_GRAPHICS_BCG_CURVE_CIRCLE_H

#include "bcg_curve.h"

namespace bcg{

template<int N>
struct curve_circle : public curve<N>{
    curve_circle() : center(VectorS<N>::Zero()), radius(1.0){

    }

    curve_circle(const VectorS<N> &center, bcg_scalar_t radius) : center(center), radius(radius){

    }

    VectorS<N> evaluate(bcg_scalar_t t) const override{
        VectorS<N> result = center;
        result[0] += radius * std::cos(t);
        result[1] += radius * std::sin(t);
        return result;
    }

    VectorS<N> derivative_vector(bcg_scalar_t t, int order) const override{
        switch (order){
            case 0 : {
                return evaluate(t);
            }
            case 1 : {
                VectorS<N> result = VectorS<N>::Zero();
                result[0] = -radius * std::sin(t);
                result[1] = radius * std::cos(t);
                return result;
            }
            case 2 : {
                VectorS<N> result = VectorS<N>::Zero();
                result[0] = -radius * std::cos(t);
                result[1] = -radius * std::sin(t);
                return result;
            }
            case 3 : {
                VectorS<N> result = VectorS<N>::Zero();
                result[0] = radius * std::sin(t);
                result[1] = -radius * std::cos(t);
                return result;
            }
            default: {
                return VectorS<N>::Zero();
            }
        }
    }

    std::vector<VectorS<N>> control_points() const override{
        return {center, evaluate(0)};
    }

    VectorS<N> center;
    bcg_scalar_t radius;
};

using curve_circle2 = curve_circle<2>;

using curve_circle3 = curve_circle<3>;

}

#endif //BCG_GRAPHICS_BCG_CURVE_CIRCLE_H
