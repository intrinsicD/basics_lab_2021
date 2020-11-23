//
// Created by alex on 23.11.20.
//

#ifndef BCG_GRAPHICS_BCG_LAPLACIAN_SMOOTHING_H
#define BCG_GRAPHICS_BCG_LAPLACIAN_SMOOTHING_H

#include "bcg_laplacian_heat_diffusion.h"

namespace bcg {

template<typename T, int N>
void laplacian_explicit_smoothing(property<T, N> p, const laplacian_matrix &laplacian, size_t iterations,
                                  bcg_scalar_t time, bool normalize = false, bool symmetric = false) {
    SparseMatrix<bcg_scalar_t> Id(p.size(), p.size());
    Id.setIdentity();

    auto dt = std::min<bcg_scalar_t>(time / iterations, 0.5);

    MatrixS<-1, N> B = MapConst(p);
    SparseMatrix<bcg_scalar_t> Op = Id + dt * laplacian.S;

    if (normalize) {
        Op = laplacian.M + dt * laplacian.S;
    }

    if (symmetric) {
        Op = laplacian.M.cwiseSqrt() + dt * laplacian.symmetric();
    }

    for (size_t i = 0; i < iterations; ++i) {
        if (normalize) {
            B = laplacian.M.cwiseInverse() * (Op * B).eval();
        } else if (symmetric) {
            B = laplacian.M.cwiseSqrt().cwiseInverse() * (Op * laplacian.M.cwiseSqrt() * B).eval();
        } else {
            B = (Op * B).eval();
        }
    }

    Map(p) = B;

    p.set_dirty();
}

template<typename T, int N>
void laplacian_implicit_smoothing(vertex_container *vertices, property<T, N> p, const laplacian_matrix &laplacian,
                                  bcg_scalar_t timestep, bool normalize = false, bool symmetric = false) {
    Map(p) = heat_diffusion(p, laplacian, timestep, normalize, symmetric);
    p.set_dirty();
}

template<typename T, int N>
void bilaplacian_smoothing(property<T, N> p, const laplacian_matrix &laplacian, bcg_scalar_t timestep) {
    SparseMatrix<bcg_scalar_t> Id(p.size(), p.size());
    Id.setIdentity();

    MatrixS< -1, N> B = MapConst(p).template cast<bcg_scalar_t>();
    SparseMatrix<bcg_scalar_t> Op = timestep * laplacian.S.transpose() * laplacian.S + laplacian.M;

    Eigen::SparseLU<SparseMatrix<bcg_scalar_t>> solver;
    solver.compute(Op);
    Map(p) = solver.solve(laplacian.M * B);
    p.set_dirty();
}

template<typename T, int N>
void laplacian_unconstrained_smoothing(property<T, N> p, const laplacian_matrix &laplacian, bcg_scalar_t timestep) {
    SparseMatrix<bcg_scalar_t> Id(p.size(), p.size());
    Id.setIdentity();

    MatrixS< -1, N> B = MapConst(p).template cast<bcg_scalar_t>();
    SparseMatrix<bcg_scalar_t> Op = laplacian.S.transpose() * laplacian.S + timestep * laplacian.M;

    Eigen::SparseLU<SparseMatrix<bcg_scalar_t>> solver;

    solver.compute(Op);
    Map(p) = solver.solve(timestep * laplacian.M * B);
    p.set_dirty();
}

}

#endif //BCG_GRAPHICS_BCG_LAPLACIAN_SMOOTHING_H
