//
// Created by alex on 04.05.21.
//

#ifndef BCG_GRAPHICS_BCG_STATISTICS_QUANTIL_H
#define BCG_GRAPHICS_BCG_STATISTICS_QUANTIL_H

#include "math/vector/bcg_vector.h"
#include <vector>

namespace bcg{

template<typename T>
struct quantil_result{
    size_t index;
    T value;
};

template<typename T>
quantil_result<T> quantil(const Vector<T, -1> &values){
    std::vector<std::pair<T, long>> dv;
    for(long i = 0; i < values.size(); ++i){
        dv.template emplace_back(values(i), i);
    }

    std::sort(dv.begin(), dv.end(),
              [](const std::pair<T, long> &lhs, const std::pair<T, long> &rhs) {
                  return lhs.first < rhs.first;
              });
    quantil_result<T> result;
    result.index = std::floor(dv.size() / 4);
    result.value = dv[result.index].first;
    return result;
}

}

#endif //BCG_GRAPHICS_BCG_STATISTICS_QUANTIL_H
