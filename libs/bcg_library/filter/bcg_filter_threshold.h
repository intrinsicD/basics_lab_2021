//
// Created by alex on 20.05.21.
//

#ifndef BCG_GRAPHICS_BCG_FILTER_THRESHOLD_H
#define BCG_GRAPHICS_BCG_FILTER_THRESHOLD_H

#include "bcg_filter.h"

namespace bcg{

struct filter_threshold_less : filter_vector<bcg_scalar_t>{
    filter_threshold_less() : filter_vector<bcg_scalar_t>([](Scalar, Scalar){
        return a < threshold;
    }){}

    ~filter_threshold_less() override = default;
};

struct filter_threshold_less_equal : filter_vector<bcg_scalar_t>{
    filter_threshold_less_equal() : filter_vector<bcg_scalar_t>([](Scalar, Scalar){
        return a <= threshold;
    }){}

    ~filter_threshold_less_equal() override = default;
};

struct filter_threshold_greater_equal : filter_vector<bcg_scalar_t>{
    filter_threshold_greater_equal() : filter_vector<bcg_scalar_t>([](Scalar, Scalar){
        return a >= threshold;
    }){}

    ~filter_threshold_greater_equal() override = default;
};

struct filter_threshold_greater : filter_vector<bcg_scalar_t>{
    filter_threshold_greater() : filter_vector<bcg_scalar_t>([](Scalar, Scalar){
        return a > threshold;
    }){}

    ~filter_threshold_greater() override = default;
};

}
#endif //BCG_GRAPHICS_BCG_FILTER_THRESHOLD_H
