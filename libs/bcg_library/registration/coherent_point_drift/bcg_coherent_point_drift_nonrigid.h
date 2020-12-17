//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_NONRIGID_H
#define BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_NONRIGID_H

#include "bcg_coherent_point_drift_base.h"

namespace bcg {

struct coherent_point_drift_nonrigid : public coherent_point_drift_base {
    MatrixS<-1, -1> P, G, T, Evecs;
    VectorS<-1> Evals;
    bcg_scalar_t beta = 2, lambda = 2;

    void init(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) override;

    void maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) override;

    void optimized_maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) override;

    VectorS<-1> transformed(const MatrixS<-1, -1> &Y, long idx) override;

    MatrixS<-1, -1> transformed(const MatrixS<-1, -1> &Y) override;
};

}

#endif //BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_NONRIGID_H
