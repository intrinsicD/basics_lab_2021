//
// Created by alex on 12.07.21.
//

#ifndef BCG_GRAPHICS_BCG_QUADRIC_FIT_H
#define BCG_GRAPHICS_BCG_QUADRIC_FIT_H

#include "bcg_probabilistic_quadric.h"
#include "math/vector/bcg_vector.h"
#include "math/matrix/bcg_matrix.h"

namespace bcg {

using quadric = pq::quadric<pq::math<bcg_scalar_t, VectorS<3>, VectorS<3>, MatrixS<3, 3>>>;

bcg_scalar_t fit(const std::vector<VectorS<3>> &points, quadric &Q);

bcg_scalar_t fit(const MatrixS<-1, 3> &points, quadric &Q);

VectorS<3> quadric_minimizer(const quadric &Q);

}

#endif //BCG_GRAPHICS_BCG_QUADRIC_FIT_H
