//
// Created by alex on 10.10.20.
//

#ifndef BCG_GRAPHICS_BCG_VECTOR_T_H
#define BCG_GRAPHICS_BCG_VECTOR_T_H

#include <cmath>
#include <array>
#include <limits>
#include <initializer_list>

namespace bcg {

template<size_t N, typename T>
struct vector {
    using reference_t = std::reference_wrapper<T>;
    using const_reference_t = std::reference_wrapper<const T>;
    using iterator_t = typename std::array<T, N>::iterator;
    using const_iterator_t = typename std::array<T, N>::const_iterator;

    std::array<T, N> data;

    vector(T value = 0);

    vector(const std::array<T, N> &other);

    vector(const vector<N - 1, T> &other);

    vector(std::initializer_list<T> values);

    inline reference_t operator[](size_t i);

    inline const_reference_t operator[](size_t i) const;

    inline reference_t operator()(size_t i);

    inline const_reference_t operator()(size_t i) const;

    inline iterator_t begin();

    inline iterator_t end();

    inline const_iterator_t begin() const;

    inline const_iterator_t end() const;

    inline reference_t front();

    inline const_reference_t front() const;

    inline reference_t back();

    inline const_reference_t back() const;

    template<size_t M, typename S>
    void copy(const vector<M, S> &other);

    static vector zero();

    static vector one();

    static vector infinity();

    // Vector comparison operations.
    inline bool operator==(const vector &other) const;

    inline bool operator!=(const vector &other) const;

    inline bool operator<(const vector &other) const;

    inline bool operator<=(const vector &other) const;

    inline bool operator>(const vector &other) const;

    inline bool operator>=(const vector &other) const;

    inline vector operator-();

    inline vector &operator+=(const vector &other);

    inline vector &operator-=(const vector &other);

    inline vector &operator*=(const vector &other);

    inline vector &operator/=(const vector &other);

    inline vector operator+(const vector &other) const;

    inline vector operator-(const vector &other) const;

    inline vector operator*(const vector &other) const;

    inline vector operator/(const vector &other) const;

    inline vector &operator+=(T value);

    inline vector &operator-=(T value);

    inline vector &operator*=(T value);

    inline vector &operator/=(T value);

    inline vector operator+(T value) const;

    inline vector operator-(T value) const;

    inline vector operator*(T value) const;

    inline vector operator/(T value) const;

    inline vector<N + 1, T> lift(size_t index = N, T value = 1) const;

    inline vector<N - 1, T> project(size_t index = N - 1) const;

    inline T dot(const vector &other) const;

    inline T squared_norm() const;

    inline T norm() const;

    inline T squared_distance(const vector &other) const;

    inline T distance(const vector &other) const;

    inline T cos(const vector &other) const;

    inline T angle(const vector &other) const;

    inline T sin(const vector &other) const;

    inline T min_coeff(size_t &index) const;

    inline T max_coeff(size_t &index) const;

    inline T min() const;

    inline T max() const;

    inline T mean() const;

    inline T sum() const;

    inline void normalize();

    inline vector &normalized();

    inline vector orthogonal();

    inline vector orthonormal();

    inline vector orthonormalize(const vector &other);

    inline vector abs();

    inline vector sqrt();

    inline vector exp();

    inline vector log();

    template<typename E>
    inline vector pow(E exponent);

    inline vector &clamped(T min, T max);

    inline vector clamp(T min, T max);

    inline vector lerp(const vector &b, T u);

    inline vector lerp(const vector &b, const vector &u);
};

template<size_t N, typename T>
vector<N, T>::vector(T value) {
    std::fill(begin(), end(), value);
}

template<size_t N, typename T>
vector<N, T>::vector(const std::array<T, N> &data) : data(data){

}

template<size_t N, typename T>
vector<N, T>::vector(const vector<N - 1, T> &other) {
    copy(other);
    back() = 1;
}

template<size_t N, typename T>
vector<N, T>::vector(std::initializer_list<T> values) {
    auto num_values = static_cast<size_t>(values.size());
    if (N == num_values) {
        std::copy(values.begin(), values.end(), begin());
    } else if (N > num_values) {
        std::copy(values.begin(), values.end(), begin());
        std::fill(begin() + num_values, end(), 0);
    } else {
        std::copy(values.begin(), values.begin() + N, begin());
    }
}

template<size_t N, typename T>
inline typename vector<N, T>::reference_t vector<N, T>::operator[](size_t i) { return data[i]; }

template<size_t N, typename T>
inline typename vector<N, T>::const_reference_t vector<N, T>::operator[](size_t i) const { return data[i]; }

template<size_t N, typename T>
inline typename vector<N, T>::reference_t vector<N, T>::operator()(size_t i) { return data[i]; }

template<size_t N, typename T>
inline typename vector<N, T>::const_reference_t vector<N, T>::operator()(size_t i) const { return data[i]; }

template<size_t N, typename T>
inline typename vector<N, T>::iterator_t vector<N, T>::begin() { return data.begin(); }

template<size_t N, typename T>
inline typename vector<N, T>::iterator_t vector<N, T>::end() { return data.end(); }

template<size_t N, typename T>
inline typename vector<N, T>::const_iterator_t vector<N, T>::begin() const { return data.begin(); }

template<size_t N, typename T>
inline typename vector<N, T>::const_iterator_t vector<N, T>::end() const { return data.end(); }

template<size_t N, typename T>
inline typename vector<N, T>::reference_t vector<N, T>::front() { return data.front(); }

template<size_t N, typename T>
inline typename vector<N, T>::const_reference_t vector<N, T>::front() const { return data.front(); }

template<size_t N, typename T>
inline typename vector<N, T>::reference_t vector<N, T>::back() { return data.back(); }

template<size_t N, typename T>
inline typename vector<N, T>::const_reference_t vector<N, T>::back() const { return data.back(); }

template<size_t N, typename T>
template<size_t M, typename S>
void vector<N, T>::copy(const vector<M, S> &other) {
    size_t _min = std::min(M, N);
    for (size_t i = 0; i < _min; ++i) {
        data[i] = other[i];
    }
}

template<size_t N, typename T>
static vector<N, T> zero() { return vector<N, T>(0); }

template<size_t N, typename T>
static vector<N, T> one() { return vector<N, T>(1); }

template<size_t N, typename T>
static vector<N, T> infinity() { return vector<N, T>(std::numeric_limits<T>::max()); }

// Vector comparison operations.
template<size_t N, typename T>
inline bool vector<N, T>::operator==(const vector &other) const {
    return data == other.data;
}

template<size_t N, typename T>
inline bool vector<N, T>::operator!=(const vector &other) const {
    return data != other.data;
}

template<size_t N, typename T>
inline bool vector<N, T>::operator<(const vector &other) const {
    return data < other.data;
}

template<size_t N, typename T>
inline bool vector<N, T>::operator<=(const vector &other) const {
    return data <= other.data;
}

template<size_t N, typename T>
inline bool vector<N, T>::operator>(const vector &other) const {
    return data > other.data;
}

template<size_t N, typename T>
inline bool vector<N, T>::operator>=(const vector &other) const {
    return data >= other.data;
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::operator-() {
    return 0 - *this;
}

template<size_t N, typename T>
inline vector<N, T> &vector<N, T>::operator+=(const vector &other) {
    const auto *ptr = other.data;
    for (auto &s : *this) {
        s += (*ptr);
        ++ptr;
    }
    return *this;
}

template<size_t N, typename T>
inline vector<N, T> &vector<N, T>::operator-=(const vector &other) {
    const auto *ptr = other.data;
    for (auto &s : *this) {
        s -= (*ptr);
        ++ptr;
    }
    return *this;
}

template<size_t N, typename T>
inline vector<N, T> &vector<N, T>::operator*=(const vector &other) {
    const auto *ptr = other.data;
    for (auto &s : *this) {
        s *= (*ptr);
        ++ptr;
    }
    return *this;
}

template<size_t N, typename T>
inline vector<N, T> &vector<N, T>::operator/=(const vector &other) {
    const auto *ptr = other.data;
    for (auto &s : *this) {
        (*ptr) != 0 ? s /= (*ptr) : s = 0;
        ++ptr;
    }
    return *this;
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::operator+(const vector &other) const {
    auto result = other;
    return result += *this;
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::operator-(const vector &other) const {
    auto result = other;
    return result -= *this;
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::operator*(const vector &other) const {
    auto result = other;
    return result *= *this;
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::operator/(const vector &other) const {
    auto result = other;
    return result /= *this;
}

template<size_t N, typename T>
inline vector<N, T> &vector<N, T>::operator+=(T value) {
    for (auto &s : *this) { s += value; }
    return *this;
}

template<size_t N, typename T>
inline vector<N, T> &vector<N, T>::operator-=(T value) {
    for (auto &s : *this) { s -= value; }
    return *this;
}

template<size_t N, typename T>
inline vector<N, T> &vector<N, T>::operator*=(T value) {
    for (auto &s : *this) { s *= value; }
    return *this;
}

template<size_t N, typename T>
inline vector<N, T> &vector<N, T>::operator/=(T value) {
    for (auto &s : *this) { value != 0 ? s /= value : s = 0; }
    return *this;
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::operator+(T value) const {
    auto result = *this;
    return result += value;
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::operator-(T value) const {
    auto result = *this;
    return result -= value;
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::operator*(T value) const {
    auto result = *this;
    return result *= value;
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::operator/(T value) const {
    auto result = *this;
    return result /= value;
}

template<size_t N, typename T>
inline vector<N + 1, T> vector<N, T>::lift(size_t index, T value) const {
    vector<N + 1, T> result(value);
    for (size_t i = 0; i < index; ++i) {
        result[i] = data[i];
    }
    for (size_t i = index + 1; i < N + 1; ++i) {
        result[i] = data[i - 1];
    }
    return result;
}

template<size_t N, typename T>
inline vector<N - 1, T> vector<N, T>::project(size_t index) const {
    vector<N - 1, T> result(0);
    for (size_t i = 0; i < index; ++i) {
        result[i] = data[i];
    }
    for (size_t i = index; i < N; ++i) {
        result[i - 1] = data[i];
    }
    return result;
}

template<size_t N, typename T>
inline T vector<N, T>::dot(const vector &other) const {
    //https://en.wikipedia.org/wiki/Kahan_summation_algorithm klein's variant.
    T sum = 0;
    T c = 0;
    T cc = 0;
    T cs = 0;
    T ccs = 0;
    const auto *ptr = other.data;
    for (const auto v : *this) {
        auto s = v * (*ptr);
        auto t = sum + s;
        if (std::abs(sum) >= std::abs(s)) {
            c = (sum - t) + s;
        } else {
            c = (s - t) + sum;
        }
        sum = t;
        t = cs + c;
        if (std::abs(cs) >= std::abs(c)) {
            cc = (cs - t) + c;
        } else {
            cc = (c - t) + cs;
        }
        cs = t;
        ccs += cc;
        ++ptr;
    }
    return sum + cs + ccs;
}

template<size_t N, typename T>
inline T vector<N, T>::squared_norm() const {
    return dot(*this);
}

template<size_t N, typename T>
inline T vector<N, T>::norm() const {
    return std::sqrt(squared_norm());
}

template<size_t N, typename T>
inline T vector<N, T>::squared_distance(const vector &other) const {
    //https://en.wikipedia.org/wiki/Kahan_summation_algorithm klein's variant.
    T sum = 0;
    T c = 0;
    T cc = 0;
    T cs = 0;
    T ccs = 0;
    const auto *ptr = other.data;
    for (const auto v : *this) {
        auto s = (v - (*ptr)) * (v - (*ptr));
        auto t = sum + s;
        if (std::abs(sum) >= std::abs(s)) {
            c = (sum - t) + s;
        } else {
            c = (s - t) + sum;
        }
        sum = t;
        t = cs + c;
        if (std::abs(cs) >= std::abs(c)) {
            cc = (cs - t) + c;
        } else {
            cc = (c - t) + cs;
        }
        cs = t;
        ccs += cc;
        ++ptr;
    }
    return sum + cs + ccs;
}

template<size_t N, typename T>
inline T vector<N, T>::distance(const vector &other) const {
    return std::sqrt(squared_distance(other));
}

template<size_t N, typename T>
inline T vector<N, T>::cos(const vector &other) const {
    return dot(other) / (norm() * other.norm());
}

template<size_t N, typename T>
inline T vector<N, T>::angle(const vector &other) const {
    return std::acos(cos(other));
}

template<size_t N, typename T>
inline T vector<N, T>::sin(const vector &other) const {
    return std::sin(angle(other));
}

template<size_t N, typename T>
inline T vector<N, T>::min_coeff(size_t &index) const {
    T _min = std::numeric_limits<T>::max();
    for (size_t i = 0; i < N; ++i) {
        if (data[i] < _min) {
            index = i;
            _min = data[i];
        }
    }
    return _min;
}

template<size_t N, typename T>
inline T vector<N, T>::max_coeff(size_t &index) const {
    T _max = std::numeric_limits<T>::min();
    for (size_t i = 0; i < N; ++i) {
        if (data[i] > _max) {
            index = i;
            _max = data[i];
        }
    }
    return _max;
}

template<size_t N, typename T>
inline T vector<N, T>::min() const {
    size_t i;
    return min_coeff(i);
}

template<size_t N, typename T>
inline T vector<N, T>::max() const {
    size_t i;
    return max_coeff(i);
}

template<size_t N, typename T>
inline T vector<N, T>::mean() const {
    return sum() / T(N);
}

template<size_t N, typename T>
inline T vector<N, T>::sum() const {
    //https://en.wikipedia.org/wiki/Kahan_summation_algorithm klein's variant.
    T sum = 0;
    T c = 0;
    T cc = 0;
    T cs = 0;
    T ccs = 0;
    for (const auto s : *this) {
        auto t = sum + s;
        if (std::abs(sum) >= std::abs(s)) {
            c = (sum - t) + s;
        } else {
            c = (s - t) + sum;
        }
        sum = t;
        t = cs + c;
        if (std::abs(cs) >= std::abs(c)) {
            cc = (cs - t) + c;
        } else {
            cc = (c - t) + cs;
        }
        cs = t;
        ccs += cc;
    }
    return sum + cs + ccs;
}

template<size_t N, typename T>
inline void vector<N, T>::normalize() {
    *this /= norm();
}

template<size_t N, typename T>
inline vector<N, T> &vector<N, T>::normalized() {
    normalize();
    return *this;
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::orthogonal() {
    auto cmax = std::abs((*this)[0]);
    size_t imax = 0;
    for (int i = 1; i < N; ++i) {
        auto c = std::abs((*this)[i]);
        if (c > cmax) {
            cmax = c;
            imax = i;
        }
    }

    auto result = zero();
    auto inext = imax + 1;
    if (inext == N) {
        inext = 0;
    }
    result[imax] = (*this)[inext];
    result[inext] = -v(*this)[imax];
    return result;
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::orthonormal() {
    return orthogonal().normalized();
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::orthonormalize(const vector &other) {
    return *this - other * dot(other);
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::abs() {
    auto result = *this;
    for (auto &s : result) {
        s = std::abs(s);
    }
    return result;
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::sqrt() {
    auto result = *this;
    for (auto &s : result) {
        s = std::sqrt(s);
    }
    return result;
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::exp() {
    auto result = *this;
    for (auto &s : result) {
        s = std::exp(s);
    }
    return result;
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::log() {
    auto result = *this;
    for (auto &s : result) {
        s = std::log(s);
    }
    return result;
}

template<size_t N, typename T>
template<typename E>
inline vector<N, T> vector<N, T>::pow(E exponent) {
    auto result = *this;
    for (auto &s : result) {
        s = std::pow(s, exponent);
    }
    return result;
}

template<size_t N, typename T>
inline vector<N, T> &vector<N, T>::clamped(T min, T max) {
    for (auto &s : *this) {
        s = std::min(std::max(s, min), max);
    }
    return *this;
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::clamp(T min, T max) {
    auto result = *this;
    return result.clamped();
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::lerp(const vector &b, T u) {
    return *this * (1 - u) + b * u;
}

template<size_t N, typename T>
inline vector<N, T> vector<N, T>::lerp(const vector &b, const vector &u) {
    return *this * (1 - u) + b * u;
}

template<size_t N, typename T, typename S>
inline vector<N, T> operator+(S value, const vector<N, T> &other) {
    return other + value;
}

template<size_t N, typename T, typename S>
inline vector<N, T> operator-(S value, const vector<N, T> &other) {
    return other - value;
}

template<size_t N, typename T, typename S>
inline vector<N, T> operator*(S value, const vector<N, T> &other) {
    return other * value;
}

template<size_t N, typename T, typename S>
inline vector<N, T> operator/(S value, const vector<N, T> &other) {
    auto result = other;
    for (auto &s : result) {
        s = (s != 0 ? value / s : 0);
    }
    return result;
}

}

#endif //BCG_GRAPHICS_BCG_VECTOR_T_H
