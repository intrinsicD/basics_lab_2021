//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_VECTOR_H
#define BCG_GRAPHICS_BCG_VECTOR_H

#include "math/matrix/bcg_matrix.h"
#include "Eigen/Geometry"

namespace bcg{

template<typename T, int N>
using Vector = Matrix<T, N, 1>;

template<int N>
using VectorS = Vector<bcg_scalar_t, N>;

template<int N>
using VectorI = Vector<bcg_index_t , N>;

// Zero vector constants.
template<int N>
BCG_MAYBE_UNUSED inline const auto zeroi = VectorI<N>::Zero();
BCG_MAYBE_UNUSED inline const auto zero2i = VectorI<2>::Zero();
BCG_MAYBE_UNUSED inline const auto zero3i = VectorI<3>::Zero();
BCG_MAYBE_UNUSED inline const auto zero4i = VectorI<4>::Zero();

// One vector constants.
template<int N>
BCG_MAYBE_UNUSED inline const auto onei = VectorI<N>::Ones();
BCG_MAYBE_UNUSED inline const auto one2i = VectorI<2>::Ones();
BCG_MAYBE_UNUSED inline const auto one3i = VectorI<3>::Ones();
BCG_MAYBE_UNUSED inline const auto one4i = VectorI<4>::Ones();

// Zero vector constants.
template<int N>
BCG_MAYBE_UNUSED inline const auto zeros = VectorS<N>::Zero();
BCG_MAYBE_UNUSED inline const auto zero2s = VectorS<2>::Zero();
BCG_MAYBE_UNUSED inline const auto zero3s = VectorS<3>::Zero();
BCG_MAYBE_UNUSED inline const auto zero4s = VectorS<4>::Zero();

// One vector constants.
template<int N>
BCG_MAYBE_UNUSED inline const auto ones = VectorS<N>::Ones();
BCG_MAYBE_UNUSED inline const auto one2s = VectorS<2>::Ones();
BCG_MAYBE_UNUSED inline const auto one3s = VectorS<3>::Ones();
BCG_MAYBE_UNUSED inline const auto one4s = VectorS<4>::Ones();

template<int N>
inline auto unit(bcg_index_t i) {
    VectorS<N> u(zeros<N>);
    u[i] = 1;
    return u;
}

}

#endif //BCG_GRAPHICS_BCG_VECTOR_H
