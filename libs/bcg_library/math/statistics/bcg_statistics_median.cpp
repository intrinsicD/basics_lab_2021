//
// Created by alex on 04.12.20.
//

#include <algorithm>
#include "bcg_statistics_median.h"

namespace bcg{

bcg_scalar_t median(std::vector<bcg_scalar_t> values){
    std::sort(values.begin(), values.end());
    size_t N = values.size();
    // check for even case
    if (N % 2 != 0){
        return values[N/ 2];
    }

    return (values[(N - 1) / 2] + values[N / 2]) / 2.0;
}

}