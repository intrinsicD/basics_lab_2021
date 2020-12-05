//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_NONRIGID_H
#define BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_NONRIGID_H

#include "math/bcg_linalg.h"

namespace bcg {

struct coherent_point_drift_nonrigid {
    MatrixS<-1, -1> P, G, T;
    VectorS<-1> P1, PT1;

    bcg_scalar_t sigma_squared, omega = 0, beta = 2, lambda = 2, N_P;
    size_t M, N, D;

    void init(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X, bcg_scalar_t omega = 0.5);

    void expectation_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X);

    void maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X);

    void operator()(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X);
};

}

#endif //BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_NONRIGID_H
