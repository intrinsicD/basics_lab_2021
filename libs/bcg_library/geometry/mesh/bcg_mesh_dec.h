//
// Created by alex on 28.12.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_DEC_H
#define BCG_GRAPHICS_BCG_MESH_DEC_H

#include "math/bcg_math_common.h"
#include "math/sparse_matrix/bcg_sparse_matrix.h"
#include "bcg_mesh.h"

namespace bcg {

struct dec_operators {
    SparseMatrix<bcg_scalar_t> hodge0, hodge1, hodge2, d0, d1;

    property<bcg_scalar_t, 1> faceAreas, vertexDualAreas, edgeCotanWeights;
};

dec_operators mesh_dec(halfedge_mesh &mesh, size_t parallel_grain_size = 1024);

}
#endif //BCG_GRAPHICS_BCG_MESH_DEC_H
