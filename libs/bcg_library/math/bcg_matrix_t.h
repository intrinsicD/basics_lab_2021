//
// Created by alex on 11.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MATRIX_T_H
#define BCG_GRAPHICS_BCG_MATRIX_T_H

#include "bcg_vector_t.h"

namespace bcg {

template<std::size_t M, std::size_t N, typename T>
struct matrix {
    using vector = vector<M, T>;
    std::array<vector, N> data;

    using reference_t = typename vector::reference_t;
    using const_reference_t = typename vector::const_reference_t;

    matrix(T value);

    matrix(const std::array<T, N * M> &data);

    matrix(std::initializer_list<T> data);

    static matrix zero();

    static matrix one();

    static matrix infinity();

    static matrix identity();

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

    inline matrix &cwise_mul(const matrix &other);

    inline matrix &operator/=(const matrix &other);

    inline matrix operator+(const matrix &other) const;

    inline matrix operator-(const matrix &other) const;

    inline matrix operator*(const matrix &other) const;

    inline matrix cwise_mul(const matrix &other) const;

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

    inline T trace() const;

    inline T determinant() const;
};

}

#endif //BCG_GRAPHICS_BCG_MATRIX_T_H
