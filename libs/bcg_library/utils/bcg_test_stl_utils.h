//
// Created by alex on 05.10.20.
//

#ifndef BCG_STL_UTILS_H
#define BCG_STL_UTILS_H

#include <vector>
#include <algorithm>

namespace bcg {

template<typename First, typename Second>
inline std::vector<std::pair<First, Second>> zip(const std::vector<First> &first,
                                                 const std::vector<Second> &second) {

    size_t min_size = std::min(first.size(), second.size());
    std::vector<std::pair<First, Second>> container(min_size);

    for (size_t i = 0; i < min_size; ++i) {
        container[i] = std::make_pair(first[i], second[i]);
    }
    return container;
}

template<typename First, typename Second>
inline void unzip(const std::vector<std::pair<First, Second>> &container,
                  std::vector<First> *first = nullptr,
                  std::vector<Second> *second = nullptr) {
    if (first != nullptr) {
        first->clear();
        first->reserve(container.size());
    }

    if (second != nullptr) {
        second->clear();
        second->reserve(container.size());
    }

    if (first == nullptr && second == nullptr) return;

    for (const auto &item : container) {
        if (first != nullptr) {
            first->emplace_back(item.first);
        }
        if (second != nullptr) {
            second->emplace_back(item.second);
        }
    }
}

template<typename First, typename Second>
inline void sort_by_first(std::vector<First> &first, std::vector<Second> &second, bool descending = false) {
    auto container = zip(first, second);
    if(descending){
        std::sort(container.begin(), container.end(), [](const std::pair<First, Second> &lhs,
                                                         const std::pair<First, Second> &rhs) {
            return lhs.first > rhs.first;
        });
    }else{
        std::sort(container.begin(), container.end(), [](const std::pair<First, Second> &lhs,
                                                         const std::pair<First, Second> &rhs) {
            return lhs.first < rhs.first;
        });
    }
    unzip<First, Second>(container, &first, &second);
}

}

#endif //BCG_STL_UTILS_H
