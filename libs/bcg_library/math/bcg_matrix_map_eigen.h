//
// Created by alex on 15.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MATRIX_MAP_EIGEN_H
#define BCG_GRAPHICS_BCG_MATRIX_MAP_EIGEN_H

#include "bcg_linalg.h"

namespace bcg {

template<int N, typename Real>
inline Eigen::Map<Eigen::Matrix<Real, -1, N>, 0, Eigen::Stride<-1, N>> Map(std::vector<Vector<Real, N>> &points) {
    return Eigen::Map<Eigen::Matrix<Real, -1, N>, 0, Eigen::Stride<-1, N>>(&points[0][0], points.size(),
                                                                           points[0].size(),
                                                                           Eigen::Stride<-1, N>(1, points[0].size()));
}

template<int N, typename Real>
inline Eigen::Map<const Eigen::Matrix<Real, -1, N>, 0, Eigen::Stride<-1, N>>
MapConst(const std::vector<Vector<Real, N>> &points) {
    return Eigen::Map<const Eigen::Matrix<Real, -1, N>, 0, Eigen::Stride<-1, N>>(&points[0][0],
                                                                                 points.size(), points[0].size(),
                                                                                 Eigen::Stride<-1, N>(1,
                                                                                                      points[0].size()));
}

template<int M, int N, typename Real>
inline Eigen::Map<Eigen::Matrix<Real, M, N>, 0, Eigen::Stride<M, N>> Map(std::array<Vector<Real, N>, M> &points) {
    return Eigen::Map<Eigen::Matrix<Real, M, N>, 0, Eigen::Stride<M, N>>(&points[0][0], M,
                                                                         points[0].size(),
                                                                         Eigen::Stride<M, N>(1, points[0].size()));
}

template<int M, int N, typename Real>
inline Eigen::Map<const Eigen::Matrix<Real, M, N>, 0, Eigen::Stride<M, N>>
MapConst(const std::array<Vector<Real, N>, M> &points) {
    return Eigen::Map<const Eigen::Matrix<Real, M, N>, 0, Eigen::Stride<M, N>>(&points[0][0], M,
                                                                               points[0].size(),
                                                                               Eigen::Stride<M, N>(1,
                                                                                                   points[0].size()));
}

}

#endif //BCG_GRAPHICS_BCG_MATRIX_MAP_EIGEN_H
