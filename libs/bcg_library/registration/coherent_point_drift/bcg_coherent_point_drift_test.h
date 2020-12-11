//
// Created by alex on 10.12.20.
//

#ifndef BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_TEST_H
#define BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_TEST_H

#include "math/bcg_linalg.h"
#include "bcg_coherent_point_drift_base.h"

namespace bcg {

struct coherent_point_drift_test : public coherent_point_drift_base {
    bcg_scalar_t s = 1.0;
    MatrixS<-1, -1> R;
    VectorS<-1> t;

    MatrixS<-1, -1> P, G, T, U, V, Evecs;
    VectorS<-1> mean_x, mean_y, Evals;

    bcg_scalar_t beta = 2, lambda = 2;

    void init(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) override;

    const MatrixS<-1, -1> &
    expectation_step(MatrixS<-1, -1> &P, const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) override;

    void maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) override;

    void optimized_expectation_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X,
                                    size_t parallel_grain_size = 1024) override;

    void optimized_maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) override;

    VectorS<-1> transformed(const MatrixS<-1, -1> &Y, long idx) override;

    MatrixS<-1, -1> transformed(const MatrixS<-1, -1> &Y) override;

    MatrixS<-1, -1> transformed_inverse(const MatrixS<-1, -1> &Y);

};

}
#endif //BCG_GRAPHICS_BCG_COHERENT_POINT_DRIFT_TEST_H
