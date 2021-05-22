//
// Created by alex on 22.05.21.
//

#ifndef BCG_GRAPHICS_BCG_VECTOR_DISTANCES_H
#define BCG_GRAPHICS_BCG_VECTOR_DISTANCES_H

#include "bcg_vector.h"

namespace bcg {

template<int D>
bcg_scalar_t metric_discrete(const VectorS<D> &x, const VectorS<D> &y) {
    if ((x - y).norm() < scalar_eps) return 0;
    return 1;
}

template<int D>
bcg_scalar_t metric_minkowski(const VectorS<D> &x, const VectorS<D> &y, int p) {
    return (x - y).array().pow(p).sum().pow(1.0 / p);
}

template<int D>
bcg_scalar_t metric_L2(const VectorS<D> &x, const VectorS<D> &y) {
    return (x-y).norm();
}

template<int D>
bcg_scalar_t metric_L1(const VectorS<D> &x, const VectorS<D> &y) {
    return (x-y).cwiseAbs().sum();
}

template<int D>
bcg_scalar_t metric_chebyshev(const VectorS<D> &x, const VectorS<D> &y) {
    return (x-y).cwiseAbs().array().maxCoeff();
}

template<int D>
bcg_scalar_t metric_canberra(const VectorS<D> &x, const VectorS<D> &y) {
    return ((x-y).cwiseAbs().array() / (x.cwiseAbs() + y.cwiseAbs()).array()).sum();
}

template<int D>
bcg_scalar_t metric_cos(const VectorS<D> &x, const VectorS<D> &y) {
    return 1 - x.normalized().transpose() * y.normalized();
}

}

#endif //BCG_GRAPHICS_BCG_VECTOR_DISTANCES_H
