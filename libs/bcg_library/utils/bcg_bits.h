//
// Created by alex on 15.10.20.
//

#ifndef BCG_GRAPHICS_BCG_BITS_H
#define BCG_GRAPHICS_BCG_BITS_H


#include <cstddef>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace bcg {
template<typename T>
inline T BIT(T pos) { return 1 << pos; }

template<typename T, typename U>
inline T DROP(T var, U pos) { return var >> pos; }

template<typename T, typename U>
inline bool CHECK_BIT(T var, U pos) { return DROP(var, pos) & 1; }

template<typename T, typename U>
inline void SET_BIT(T &var, U pos) { var |= BIT(pos); }

template<typename T, typename U>
inline void RESET_BIT(T &var, U pos) { var &= ~BIT(pos); }

template<typename T, typename U>
inline void TOGGLE_BIT(T &var, U pos) { var ^= BIT(pos); }

//                  last
//                    v
// count: 8 7 6 5 4 3 2 1
// pos:   7 6 5 4 3 2 1 0
// value: 0 0 0 0 0 1 1 0
template<typename T, typename U>
inline T LAST(T var, U pos) { return var & (BIT(pos) - 1); }

template<typename T, typename U>
inline T MID(T var, U from, U offset) { return LAST(DROP(var, from), offset); }

inline size_t COUNTSETBITS(size_t v) {
    v = v - ((v >> 1) & 0x5555555555555555UL);
    v = (v & 0x3333333333333333UL) + ((v >> 2) & 0x3333333333333333UL);
    return (int) ((((v + (v >> 4)) & 0xF0F0F0F0F0F0F0FUL) * 0x101010101010101UL) >> 56);
}

inline size_t clz(size_t x) {
    int n = 64;
    size_t y;

    y = x >> 32;
    if (y != 0) {
        n = n - 32;
        x = y;
    }
    y = x >> 16;
    if (y != 0) {
        n = n - 16;
        x = y;
    }
    y = x >> 8;
    if (y != 0) {
        n = n - 8;
        x = y;
    }
    y = x >> 4;
    if (y != 0) {
        n = n - 4;
        x = y;
    }
    y = x >> 2;
    if (y != 0) {
        n = n - 2;
        x = y;
    }
    y = x >> 1;
    if (y != 0) return n - 2;
    return n - x;
}

template<typename T>
inline size_t RMSB(T var) { return var == 0 ? 0 : std::log2((var) & -(var)) + 1; }

template<typename T>
inline size_t LMSB(T var) { return var == 0 ? 0 : 64 - clz(var); }

inline bool FSIGN(float var) { return var < 0 ? true : false; }

inline int FEXPONENT(float var) {
    int e;
    frexp(var, &e);
    return e;
}

inline double FMANTISSA(float var) {
    int e;
    return frexp(var, &e);
}

struct BIT_REP {
    std::uint64_t bits;
    bool sign;
    unsigned int exp;
    unsigned long mant;
};

inline BIT_REP to_bits(float value) {
    std::uint32_t tmp;
    std::memcpy(&tmp, &value, sizeof(float));
    return {tmp,
            (tmp & 0x80000000) != 0,
            (unsigned int) ((tmp & 0x7F800000) >> 23) - 126 - 24,
            (tmp & 0x007FFFFF) | 0x00800000};
}

inline BIT_REP to_bits(double value) {
    std::uint64_t tmp;
    std::memcpy(&tmp, &value, sizeof(double));
    return {tmp,
            (tmp & 0x8000000000000000) != 0,
            (unsigned int) ((tmp & 0x7FF0000000000000) >> 52) - 1022 - 53,
            (tmp & 0x000FFFFFFFFFFFFF) | 0x0010000000000000};
}

inline float bin_to_float(std::uint32_t x) {
    float f = 0.0f;
    memcpy(&f, &x, sizeof(f) < sizeof(x) ? sizeof(f) : sizeof(x));
    return f;
}

inline double bin_to_double(std::uint64_t x) {
    double d = 0.0f;
    memcpy(&d, &x, sizeof(d) < sizeof(x) ? sizeof(d) : sizeof(x));
    return d;
}

template<typename T>
inline T XOR(T var1, T var2) { return var1 ^ var2; }

template<typename T>
inline size_t HAMMING(T var1, T var2) { return COUNTSETBITS(XOR(var1, var2)); }

}

#endif //BCG_GRAPHICS_BCG_BITS_H
