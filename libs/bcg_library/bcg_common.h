//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_COMMON_H
#define BCG_GRAPHICS_BCG_COMMON_H

// -----------------------------------------------------------------------------
// INCLUDES
// -----------------------------------------------------------------------------

#include <chrono>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------
// TIMING UTILITIES
// -----------------------------------------------------------------------------
namespace bcg {

// get time in nanoseconds - useful only to compute difference of times
inline int64_t get_time();

}  // namespace bcg

// -----------------------------------------------------------------------------
// PYTHON-LIKE ITERATORS
// -----------------------------------------------------------------------------
namespace bcg {

// Python `range()` equivalent. Construct an object that c over an
// integer sequence.
template<typename T>
inline auto range(T min, T max);

template<typename T>
inline auto range(T max);

// Python `enumerate()` equivalent. Construct an object that iterates over a
// sequence of elements and numbers them.
template<typename T>
inline auto enumerate(const std::vector<T> &vals);

template<typename T>
inline auto enumerate(std::vector<T> &vals);

// Vector append and concatenation
template<typename T>
inline std::vector<T> &append(std::vector<T> &a, const std::vector<T> &b);

template<typename T>
inline std::vector<T> &append(std::vector<T> &a, const T &b);

template<typename T>
inline std::vector<T> join(const std::vector<T> &a, const std::vector<T> &b);

template<typename T>
inline std::vector<T> join(const std::vector<T> &a, const T &b);

}

// -----------------------------------------------------------------------------
//
//
// IMPLEMENTATION
//
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// TIMING UTILITIES
// -----------------------------------------------------------------------------
namespace bcg {

// get time in nanoseconds - useful only to compute difference of times
inline int64_t get_time() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

}  // namespace bcg

// -----------------------------------------------------------------------------
// PYTHON-LIKE ITERATORS
// -----------------------------------------------------------------------------
namespace bcg {

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

    T begin_ = 0, end_ = 0;

    iterator begin() const { return {begin_}; }

    iterator end() const { return {end_}; }
};

// Python `range()` equivalent. Construct an object to iterate over a sequence.
template<typename T>
inline auto range(T min, T max) {
    return range_helper{min, max};
}

template<typename T>
inline auto range(T max) {
    return range((T) 0, max);
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

        std::pair<int64_t &, T &> operator*() const { return {pos, *(data + pos)}; }
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

#endif //BCG_GRAPHICS_BCG_COMMON_H
