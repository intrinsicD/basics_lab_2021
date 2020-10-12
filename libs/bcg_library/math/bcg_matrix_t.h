//
// Created by alex on 11.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MATRIX_T_H
#define BCG_GRAPHICS_BCG_MATRIX_T_H

#include <memory>
#include "bcg_array.h"

namespace bcg {

template<std::size_t M, std::size_t N, typename T>
struct matrix {
    using vector = vector<M, T>;
    using storage_t = std::array<vector, N>;

    std::shared_ptr<storage_t> data_ptr;

    using iterator_t = typename storage_t::iterator_t;
    using const_iterator_t = typename storage_t::const_iterator_t;
    using reference_t = typename vector::reference_t;
    using const_reference_t = typename vector::const_reference_t;

    matrix(T value);

    matrix(const std::array<T, N * M> &data);

    matrix(std::initializer_list<T> data);

    array<M, N, T> to_array() const;

private:
    friend array<M, N, T>;

    matrix(const std::shared_ptr<storage_t> &other) : data_ptr(other) { }

    static matrix zero();

    static matrix one();

    static matrix infinity();

    static matrix identity();

    static matrix random();

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

    inline bool operator==(const matrix &other) const;

    inline bool operator!=(const matrix &other) const;

    inline bool operator<(const matrix &other) const;

    inline bool operator<=(const matrix &other) const;

    inline bool operator>(const matrix &other) const;

    inline bool operator>=(const matrix &other) const;

    inline matrix &operator+=(const matrix &other);

    inline matrix &operator-=(const matrix &other);

    inline matrix &operator*=(const matrix &other);

    inline matrix &operator/=(const matrix &other);

    inline matrix operator+(const matrix &other) const;

    inline matrix operator-(const matrix &other) const;

    inline matrix operator*(const matrix &other) const;

    inline matrix operator/(const matrix &other) const;

    inline matrix &operator+=(const T value);

    inline matrix &operator-=(const T value);

    inline matrix &operator*=(const T value);

    inline matrix &operator/=(const T value);

    inline matrix operator+(const T value) const;

    inline matrix operator-(const T value) const;

    inline matrix operator*(const T value) const;

    inline matrix operator/(const T value) const;

    inline vector operator*(const vector &other) const;

    inline matrix &transposed();

    inline matrix transpose() const;

    inline matrix inverse() const;

    inline matrix pseudo_inverse() const;

    inline T trace() const;

    inline T determinant() const;

    inline matrix sqrt() const;

    inline matrix square() const;

    inline matrix exp() const;

    inline matrix log() const;

    template<typename E>
    inline matrix pow(E exponent) const;
};

}

#endif //BCG_GRAPHICS_BCG_MATRIX_T_H
