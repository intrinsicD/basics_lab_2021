//
// Created by alex on 17.06.21.
//

#ifndef BCG_GRAPHICS_BCG_OBJECT_SPACE_H
#define BCG_GRAPHICS_BCG_OBJECT_SPACE_H

#include "math/matrix/bcg_matrix.h"
#include "math/bcg_linalg.h"

namespace bcg{

VectorS<3> object_space_compute_center(const MatrixS<-1, 3> &data);

VectorS<3> object_space_compute_scale(const MatrixS<-1, 3> &data);

bcg_scalar_t object_space_compute_uniform_scale(const MatrixS<-1, 3> &data);

}

#endif //BCG_GRAPHICS_BCG_OBJECT_SPACE_H
