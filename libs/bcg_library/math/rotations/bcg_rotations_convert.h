//
// Created by alex on 24.09.20.
//

#ifndef BCG_CONVERTROTATIONS_H
#define BCG_CONVERTROTATIONS_H

#include "math/bcg_linalg.h"

namespace bcg {

inline std::vector<MatrixS<3, 3>>
convert_to_rotationmatrices(const std::vector<Quaternion> &rotations) {
    std::vector<MatrixS<3, 3>> matrices;
    for (const Quaternion &q : rotations) {
        matrices.emplace_back(q.toRotationMatrix());
    }
    return matrices;
}

inline std::vector<Quaternion>
convert_to_quaternions(const std::vector<MatrixS<3, 3>> &rotations) {
    std::vector<Quaternion> quaternions;
    for (const MatrixS<3, 3> &r : rotations) {
        quaternions.emplace_back(r);
    }
    return quaternions;
}
}

#endif //BCG_CONVERTROTATIONS_H
