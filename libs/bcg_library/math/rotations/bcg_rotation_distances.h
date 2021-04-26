//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_ROTATION_DISTANCES_H
#define BCG_GRAPHICS_BCG_ROTATION_DISTANCES_H

#include "bcg_rotation_matrix_logarithm.h"
#include "math/bcg_math_safe_tigonometric_functions.h"
#include "math/bcg_linalg.h"

namespace bcg {

inline bcg_scalar_t angular_distance(const MatrixS<3, 3> &R1,
                                     const MatrixS<3, 3> &R2) {
    return matrix_logarithm(R1.transpose() * R2).norm();
}

inline bcg_scalar_t angular_distance(const Quaternion &R1,
                                    const Quaternion &R2) {
    return 2 * safe_acos(std::abs((R2.conjugate() * R1).w()));
}

inline bcg_scalar_t chordal_distance(const MatrixS<3, 3> &R1,
                                    const MatrixS<3, 3> &R2) {
    return (R2 - R1).norm();
}

inline bcg_scalar_t chordal_distance(bcg_scalar_t angle) {
    return 2.0f * std::sqrt(2.0f) * std::sin(angle / 2.0f);
}

inline bcg_scalar_t quaternion_distance(const Quaternion &r1,
                                       const Quaternion &r2) {
    return std::min((r1.coeffs() - r2.coeffs()).norm(), (r1.coeffs() + r2.coeffs()).norm());
}

inline bcg_scalar_t quaternion_distance(bcg_scalar_t angle) {
    return 2 * std::sin(angle / 4);
}

inline bcg_scalar_t quaternion_distance(const MatrixS<3, 3> &R1,
                                       const MatrixS<3, 3> &R2) {
    return quaternion_distance(angular_distance(R1, R2));
}

}

#endif //BCG_GRAPHICS_BCG_ROTATION_DISTANCES_H
