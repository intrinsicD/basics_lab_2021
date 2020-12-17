//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_AFFINE_H
#define BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_AFFINE_H

#include "bcg_coherent_point_drift_base.h"

namespace bcg {

struct coherent_point_drift_affine : public coherent_point_drift_base{
    MatrixS<-1, -1> B;
    VectorS<-1> t;

    MatrixS<-1, -1> P;
    VectorS<-1> mean_x, mean_y;

    void init(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) override;

    void maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) override;

    void optimized_maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) override;

    VectorS<-1> transformed(const MatrixS<-1, -1> &Y, long idx) override;
};

}

#endif //BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_AFFINE_H
