//
// Created by alex on 13.11.20.
//

#ifndef BCG_GRAPHICS_BCG_CURVE_HELIX_H
#define BCG_GRAPHICS_BCG_CURVE_HELIX_H


#include "bcg_curve.h"

namespace bcg{

struct curve_helix3 : public curve<3>{
    curve_helix3() : parameters(VectorS<2>::Zero()){

    }

    curve_helix3(bcg_scalar_t radius, bcg_scalar_t slope) : parameters(radius, slope * radius){

    }

    VectorS<3> evaluate(bcg_scalar_t t) const override{
        VectorS<3> result;
        result[0] = parameters[0] * std::cos(t);
        result[1] = parameters[0] * std::sin(t);
        result[2] = parameters[1] * t;
        return result;
    }

    VectorS<3> derivative_vector(bcg_scalar_t t, int order) const override{
        switch (order){
            case 0 : {
                return evaluate(t);
            }
            case 1 : {
                VectorS<3> result;
                result[0] = -parameters[0] * std::sin(t);
                result[1] = parameters[0] * std::cos(t);
                result[2] = parameters[1];
                return result;
            }
            case 2 : {
                VectorS<3> result;
                result[0] = -parameters[0] * std::cos(t);
                result[1] = -parameters[0] * std::sin(t);
                result[2] = 0;
                return result;
            }
            case 3 : {
                VectorS<3> result;
                result[0] = parameters[0] * std::sin(t);
                result[1] = -parameters[0] * std::cos(t);
                result[2] = 0;
                return result;
            }
            default: {
                return VectorS<3>::Zero();
            }
        }
    }

    std::vector<VectorS<3>> control_points() const override{
        return {{0, 0, 0.0}, evaluate(0.0), {0, 0, parameters[1]}};
    }

    VectorS<2> parameters;
};

}

#endif //BCG_GRAPHICS_BCG_CURVE_HELIX_H
