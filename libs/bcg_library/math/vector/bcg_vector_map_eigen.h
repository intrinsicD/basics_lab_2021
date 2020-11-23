//
// Created by alex on 15.10.20.
//

#ifndef BCG_GRAPHICS_BCG_VECTOR_MAP_EIGEN_H
#define BCG_GRAPHICS_BCG_VECTOR_MAP_EIGEN_H

#include "Eigen/Core"
#include "math/bcg_linalg.h"

namespace bcg {

template<typename Real>
inline Eigen::Map<Vector<Real, -1>> Map(std::vector<Real> &points) {
    return Eigen::Map<Vector<Real, -1>>(points.data(), points.size(), 1);
}

template<typename Real>
inline Eigen::Map<const Vector<Real, -1>> MapConst(const std::vector<Real> &points) {
    return Eigen::Map<const Vector<Real, -1>>(points.data(), points.size(), 1);
}

}
#endif //BCG_GRAPHICS_BCG_VECTOR_MAP_EIGEN_H
