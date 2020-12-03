//
// Created by alex on 03.12.20.
//

#ifndef BCG_GRAPHICS_BCG_GAUSSIAN_H
#define BCG_GRAPHICS_BCG_GAUSSIAN_H

#include "bcg_linalg.h"

namespace bcg{

template<int N>
inline bcg_scalar_t mahalonobis_distance_squared(const VectorS<N> &x, const VectorS<N> &y, const MatrixS<N, N> &cov_inv) {
    return (x - y).transpose() * cov_inv * (x - y);
}

inline bcg_scalar_t gaussian(bcg_scalar_t distance_squared) {
    return std::exp(-distance_squared / 2.0);
}

inline bcg_scalar_t gaussian(bcg_scalar_t distance_squared, bcg_scalar_t sigma_squared) {
    return gaussian(distance_squared / sigma_squared);
}

template<int N>
inline bcg_scalar_t gaussian(const VectorS<N> &x, const VectorS<N> &mean, const MatrixS<N, N> &cov_inv) {
    return gaussian(mahalonobis_distance_squared<N>(x, mean, cov_inv));
}

inline bcg_scalar_t gaussian_normalizer(bcg_scalar_t sigma_squared){
    return 1.0 / std::sqrt(2.0 * pi * sigma_squared);
}

}

#endif //BCG_GRAPHICS_BCG_GAUSSIAN_H
