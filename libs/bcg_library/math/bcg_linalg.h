//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_LINALG_H
#define BCG_GRAPHICS_BCG_LINALG_H

#include "matrix/bcg_matrix.h"
#include "vector/bcg_vector.h"

namespace bcg {

template<typename Derived>
using Ref = Eigen::DenseBase<Derived>;

using Transform = Eigen::Transform<bcg_scalar_t, 3, Eigen::Affine>;

using Translation = Eigen::Translation<bcg_scalar_t, 3>;

using Rotation = Eigen::AngleAxis<bcg_scalar_t>;

using Scaling = Eigen::DiagonalMatrix<bcg_scalar_t, 3>;

using Quaternion = Eigen::Quaternion<bcg_scalar_t>;

}


#endif //BCG_GRAPHICS_BCG_LINALG_H
