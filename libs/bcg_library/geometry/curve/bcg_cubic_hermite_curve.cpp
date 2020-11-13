//
// Created by alex on 12.11.20.
//

#include "bcg_cubic_hermite_curve.h"

namespace bcg{

cubic_hermite_curve::cubic_hermite_curve(){

}

VectorS<3> cubic_hermite_curve::evaluate(bcg_scalar_t t){
    return (1 - t) * (1 - t) * (1 + 2 * t) * positions[0] +
           (positions[1] - positions[0]) * t * (1 - t) * (1 - t) +
           (positions[4] - positions[3]) * (-t * t) * (1 - t) +
           positions[4] * (3 - 2 * t) * t * t;
}

cubic_hermite_curve cubic_hermite_curve::derivative(int order){

}

VectorS<3> cubic_hermite_curve::derivative(bcg_scalar_t t, int order){
    if(order == 1){
        return 6 * t * (t - 1) * positions[0] +
               (positions[1] - positions[0]) * (3 * t * t - 4 * t + 1) +
               (positions[4] - positions[3]) * (3 * t * t - 2 * t) +
               positions[4] * (6 * t * (1 - t));
    }else if(order == 2){
        return (12 * t - 6) * positions[0] +
               (positions[1] - positions[0]) * (6 * t - 4) +
               (positions[4] - positions[3]) * (6 * t - 2) +
               positions[4] * (6 - 12 * t);
    }
    return VectorS<3>::Zero();
}

}