//
// Created by alex on 12.10.20.
//

#ifndef BCG_GRAPHICS_BCG_ARRAY_H
#define BCG_GRAPHICS_BCG_ARRAY_H

#include <memory>
#include "bcg_vector_t.h"

namespace bcg{

template<std::size_t M, std::size_t N, typename T>
struct matrix;

template<std::size_t M, std::size_t N, typename T>
struct array{
    using vector = vector<M, T>;
    using storage_t = std::array<vector, N>;

    using iterator_t = typename storage_t::iterator_t;
    using const_iterator_t = typename storage_t::const_iterator_t;
    using reference_t = typename vector::reference_t;
    using const_reference_t = typename vector::const_reference_t;

    std::shared_ptr<storage_t> data_ptr;

    array(T value);

    array(const std::array<T, N * M> &data);

    array(std::initializer_list<T> data);

    matrix<M, N, T> to_matrix() const;

    static array zero();

    static array one();

    static array infinity();

    static array identity();

    static array random();

    reference_t &operator[](size_t i);

    const_reference_t &operator[](size_t i) const;

    reference_t &operator()(size_t row, size_t col);

    const_reference_t &operator()(size_t row, size_t col) const;

    size_t rows() const;

    size_t cols() const;

    vector &row(size_t row);

    const vector &row(size_t row) const;

    vector &col(size_t row);

    const vector &col(size_t row) const;

    size_t size() const;

    inline bool operator==(const array &other) const;

    inline bool operator!=(const array &other) const;

    inline bool operator<(const array &other) const;

    inline bool operator<=(const array &other) const;

    inline bool operator>(const array &other) const;

    inline bool operator>=(const array &other) const;

    inline array &operator+=(const array &other);

    inline array &operator-=(const array &other);

    inline array &operator*=(const array &other);

    inline array &operator/=(const array &other);

    inline array operator+(const array &other) const;

    inline array operator-(const array &other) const;

    inline array operator*(const array &other) const;

    inline array operator/(const array &other) const;

    inline array &operator+=(const T value);

    inline array &operator-=(const T value);

    inline array &operator*=(const T value);

    inline array &operator/=(const T value);

    inline array operator+(const T value) const;

    inline array operator-(const T value) const;

    inline array operator*(const T value) const;

    inline array operator/(const T value) const;

    inline T min_coeff(size_t &index) const;

    inline T max_coeff(size_t &index) const;

    inline T min() const;

    inline T max() const;

    inline T mean() const;

    inline T sum() const;

    inline array sqrt() const;

    inline array square() const;

    inline array exp() const;

    inline array log() const;

    template<typename E>
    inline array pow(E exponent) const;

    inline array &clamped(T min, T max);

    inline array clamp(T min, T max) const;

    inline array lerp(const array &b, T u) const;

    inline array lerp(const array &b, const array &u) const;

private:
    friend matrix<M, N, T>;
    array(std::shared_ptr<storage_t> data) : data_ptr(data) {}
};

}

#endif //BCG_GRAPHICS_BCG_ARRAY_H
