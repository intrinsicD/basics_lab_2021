//
// Created by alex on 15.10.20.
//

#ifndef BCG_GRAPHICS_MAPTOEIGEN_H
#define BCG_GRAPHICS_MAPTOEIGEN_H

#include "Vector.h"
#include "Matrix.h"
#include <vector>
#include "exts/eigen/Eigen/Core"

namespace bcg {

template<int N, typename Real>
inline Eigen::Map<Eigen::Matrix<Real, -1, N>, 0, Eigen::Stride<-1, N>> Map(std::vector<Vector<N, Real>> &points) {
    return Eigen::Map<Eigen::Matrix<Real, -1, N>, 0, Eigen::Stride<-1, N>>(&points[0][0], points.size(),
                                                                           points[0].GetSize(),
                                                                           Eigen::Stride<-1, N>(1,
                                                                                                points[0].GetSize()));
}

template<int N, typename Real>
inline Eigen::Map<const Eigen::Matrix<Real, -1, N>, 0, Eigen::Stride<-1, N>>
MapConst(const std::vector<Vector<N, Real>> &points) {
    return Eigen::Map<const Eigen::Matrix<Real, -1, N>, 0, Eigen::Stride<-1, N>>(&points[0][0],
                                                                                 points.size(), points[0].GetSize(),
                                                                                 Eigen::Stride<-1, N>(1,
                                                                                                      points[0].GetSize()));
}

template<int N, typename Real>
inline Eigen::Map<Eigen::Vector<Real, N>> Map(Vector<N, Real> &points) {
    return Eigen::Map<Eigen::Vector<Real, N>>(&points[0], points.GetSize(), 1);
}

template<int N, typename Real>
inline Eigen::Map<const Eigen::Vector<Real, N>> MapConst(const Vector<N, Real> &points) {
    return Eigen::Map<const Eigen::Vector<Real, N>>(&points[0], points.GetSize(), 1);
}

template<int M, int N, typename Real>
inline Eigen::Map<Eigen::Matrix<Real, M, N>> Map(Matrix<M, N, Real> &points) {
    return Eigen::Map<Eigen::Matrix<Real, M, N>>(&points[0], M, N);
}

template<int M, int N, typename Real>
inline Eigen::Map<const Eigen::Matrix<Real, M, N>> MapConst( const Matrix<M, N, Real> &points) {
    return Eigen::Map<const Eigen::Matrix<Real, M, N>>(&points[0], M, N);
}

}

#endif //BCG_GRAPHICS_MAPTOEIGEN_H
