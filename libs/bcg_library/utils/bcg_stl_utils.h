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

// Range object to support Python-like iteration. Use with `range()`.
template<typename T>
struct range_helper {
    struct iterator {
        T pos = 0;

        iterator &operator++() {
            ++pos;
            return *this;
        }

        bool operator!=(const iterator &other) const { return pos != other.pos; }

        T operator*() const { return pos; }
    };

    range_helper(T min, T max) : begin_(min), end_(max) {}

    T begin_ = 0, end_ = 0;

    iterator begin() const { return {begin_}; }

    iterator end() const { return {end_}; }
};

// Python `range()` equivalent. Construct an object to iterate over a sequence.
template<typename T>
inline auto range(T min, T max) {
    return range_helper{min, max};
}


// Enumerate object to support Python-like enumeration. Use with `enumerate()`.
template<typename T>
struct enumerate_helper {
    struct iterator {
        T *data = nullptr;
        int64_t pos = 0;

        iterator &operator++() {
            ++pos;
            return *this;
        }

        bool operator!=(const iterator &other) const { return pos != other.pos; }

        std::pair<int64_t, T> operator*() const { return {pos, *(data + pos)}; }
    };

    T *data = nullptr;
    int64_t size = 0;

    iterator begin() const { return {data, 0}; }

    iterator end() const { return {data, size}; }
};

// Python `enumerate()` equivalent. Construct an object that iteraterates over a
// sequence of elements and numbers them.
template<typename T>
inline auto enumerate(const std::vector<T> &vals) {
    return enumerate_helper<const T>{vals.data(), (int64_t) vals.size()};
}

template<typename T>
inline auto enumerate(std::vector<T> &vals) {
    return enumerate_helper<T>{vals.data(), (int64_t) vals.size()};
}

// Vector append and concatenation
template<typename T>
inline std::vector<T> &append(std::vector<T> &a, const std::vector<T> &b) {
    a.insert(a.end(), b.begin(), b.end());
    return a;
}

template<typename T>
inline std::vector<T> &append(std::vector<T> &a, const T &b) {
    a.push_back(b);
    return a;
}

template<typename T>
inline std::vector<T> join(const std::vector<T> &a, const std::vector<T> &b) {
    auto c = a;
    return append(c, b);
}

template<typename T>
inline std::vector<T> join(const std::vector<T> &a, const T &b) {
    auto c = a;
    return append(c, b);
}

}

#endif //BCG_STL_UTILS_H
