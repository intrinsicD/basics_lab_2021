//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_LINALG_H
#define BCG_GRAPHICS_BCG_LINALG_H

#include "exts/eigen/Eigen/Core"
#include "exts/eigen/Eigen/Geometry"
#include "bcg_math_common.h"

namespace bcg {

template<typename T, bcg_index_t M, bcg_index_t N>
using Matrix = Eigen::Matrix<T, M, N>;

template<bcg_index_t M, bcg_index_t N>
using MatrixS = Matrix<bcg_scalar_t, M, N>;

template<typename T, bcg_index_t N>
using Vector = Matrix<T, N, 1>;

template<bcg_index_t N>
using VectorS = Vector<bcg_scalar_t, N>;

template<bcg_index_t N>
using VectorI = Vector<bcg_index_t, N>;

// Zero vector constants.
template<bcg_index_t N>
[[maybe_unused]] inline const auto zeroi = VectorI<N>::Zero();
[[maybe_unused]] inline const auto zero2i = VectorI<2>::Zero();
[[maybe_unused]] inline const auto zero3i = VectorI<3>::Zero();
[[maybe_unused]] inline const auto zero4i = VectorI<4>::Zero();

// One vector constants.
template<bcg_index_t N>
[[maybe_unused]] inline const auto onei = VectorI<N>::Ones();
[[maybe_unused]] inline const auto one2i = VectorI<2>::Ones();
[[maybe_unused]] inline const auto one3i = VectorI<3>::Ones();
[[maybe_unused]] inline const auto one4i = VectorI<4>::Ones();

// Zero vector constants.
template<bcg_index_t N>
[[maybe_unused]] inline const auto zeros = VectorS<N>::Zero();
[[maybe_unused]] inline const auto zero2s = VectorS<2>::Zero();
[[maybe_unused]] inline const auto zero3s = VectorS<3>::Zero();
[[maybe_unused]] inline const auto zero4s = VectorS<4>::Zero();

// One vector constants.
template<bcg_index_t N>
[[maybe_unused]] inline const auto ones = VectorS<N>::Ones();
[[maybe_unused]] inline const auto one2s = VectorS<2>::Ones();
[[maybe_unused]] inline const auto one3s = VectorS<3>::Ones();
[[maybe_unused]] inline const auto one4s = VectorS<4>::Ones();

template<bcg_index_t N>
inline auto unit(bcg_index_t i) {
    VectorS<N> u(zeros<N>);
    u[i] = 1;
    return u;
}

}


#endif //BCG_GRAPHICS_BCG_LINALG_H
