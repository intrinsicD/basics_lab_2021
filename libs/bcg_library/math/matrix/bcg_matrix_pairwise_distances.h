//
// Created by alex on 04.02.21.
//

#ifndef BCG_GRAPHICS_BCG_MATRIX_PAIRWISE_DISTANCES_H
#define BCG_GRAPHICS_BCG_MATRIX_PAIRWISE_DISTANCES_H

#include "math/matrix/bcg_matrix.h"

namespace bcg {

template<typename Derived>
inline Matrix<typename Derived::Scalar, -1, -1> pairwise_distance_squared(const Eigen::EigenBase<Derived> &A, const Eigen::EigenBase<Derived>  &B) {
    typedef typename Derived::Scalar Scalar;
    return Vector<Scalar, -1>::Ones(A.derived().rows()) * B.derived().rowwise().squaredNorm().transpose()
           - (Scalar(2.0) * A.derived()) * B.derived().transpose() +
           A.derived().rowwise().squaredNorm() * Vector<Scalar, -1>::Ones(B.derived().rows()).transpose();
}

/*template<typename T>
inline Matrix<T, -1, -1> pairwise_distance_squared(const Matrix<T, -1, -1> &A, const Matrix<T, -1, -1> &B) {
    return Vector<T, -1>::Ones(A.rows()) * B.rowwise().squaredNorm().transpose()
           - (2 * A) * B.transpose() +
           A.rowwise().squaredNorm() * Vector<T, -1>::Ones(B.rows()).transpose();
}*/

template<typename T>
inline Matrix<T, -1, -1> pairwise_distance(const Matrix<T, -1, -1> &A, const Matrix<T, -1, -1> &B) {
    return (Vector<T, -1>::Ones(A.rows()) * B.rowwise().squaredNorm().transpose()
            - (2 * A) * B.transpose() +
            A.rowwise().squaredNorm() * Vector<T, -1>::Ones(B.rows()).transpose()).array().sqrt();
}

}

#endif //BCG_GRAPHICS_BCG_MATRIX_PAIRWISE_DISTANCES_H
