//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_LAPLACIAN_HEAT_DIFFUSION_H
#define BCG_GRAPHICS_BCG_LAPLACIAN_HEAT_DIFFUSION_H

#include "bcg_property.h"
#include "bcg_laplacian_matrix.h"
#include "bcg_property_map_eigen.h"
#include "math/matrix/bcg_matrix.h"
#include "Eigen/SparseLU"

namespace bcg {

template<typename T, int N>
MatrixS<-1, N> heat_diffusion(property<T, N> p, const laplacian_matrix &laplacian, bcg_scalar_t timestep,
                              bool normalize = false, bool symmetric = false) {
    SparseMatrix<bcg_scalar_t> Id(p.size(), p.size());
    Id.setIdentity();

    MatrixS<-1, N> B = MapConst(p).template cast<bcg_scalar_t>();
    SparseMatrix<bcg_scalar_t> Op = Id - timestep * laplacian.S;

    if (normalize) {
        Op = laplacian.M - timestep * laplacian.S;
    }
    if (symmetric) {
        Op = Id - timestep * laplacian.symmetric();
    }

    Eigen::SparseLU<SparseMatrix<bcg_scalar_t>> solver;

    solver.compute(Op);

    if (normalize) {
        B = solver.solve(laplacian.M * B);
    } else if (symmetric) {
        B = laplacian.M.cwiseSqrt().cwiseInverse() * solver.solve(laplacian.M.cwiseSqrt() * B);
    } else {
        B = solver.solve(B);
    }

    return B;
}

}

#endif //BCG_GRAPHICS_BCG_LAPLACIAN_HEAT_DIFFUSION_H
