//
// Created by alex on 05.12.20.
//

#include "bcg_coherent_point_drift_rigid.h"
#include "math/rotations/bcg_rotation_project_on_so.h"
#include "tbb/tbb.h"

namespace bcg {

void coherent_point_drift_rigid::init(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    coherent_point_drift_base::init(Y, X);

    s = 1.0;
    R = MatrixS<-1, -1>::Identity(D, D);
    t = VectorS<-1>::Zero(D);

    if(!optimized){
        P = MatrixS<-1, -1>::Zero(M, N);
    }
}

const MatrixS<-1, -1> &coherent_point_drift_rigid::expectation_step(MatrixS<-1, -1> &P, const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    return coherent_point_drift_base::expectation_step(P, Y, X);
}

void coherent_point_drift_rigid::maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X) {
    mean_x = X.transpose() * PT1 / N_P;
    mean_y = Y.transpose() * P1 / N_P;
    MatrixS<-1, -1> A(PX.transpose() * Y - N_P * mean_x * mean_y.transpose());
    R = project_on_so(A, true);
    bcg_scalar_t ARtrace = (A.transpose() * R).trace();
    s = ARtrace / ((P1.array() * Y.rowwise().squaredNorm().array()).sum() - N_P * mean_y.squaredNorm());
    t = mean_x - s * R * mean_y;
    sigma_squared = ((PT1.array() * X.rowwise().squaredNorm().array()).sum() - N_P * mean_x.squaredNorm() - s * ARtrace) / (N_P * D);
    sigma_squared = std::max<bcg_scalar_t>(sigma_squared, scalar_eps);
}

void coherent_point_drift_rigid::optimized_expectation_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X, size_t parallel_grain_size){
    coherent_point_drift_base::optimized_expectation_step(Y, X, parallel_grain_size);
}

void coherent_point_drift_rigid::optimized_maximization_step(const MatrixS<-1, -1> &Y, const MatrixS<-1, -1> &X){
    maximization_step(Y, X);
}

VectorS<-1> coherent_point_drift_rigid::transformed(const MatrixS<-1, -1> &Y, long idx){
    return s * R * Y.row(idx).transpose() + t;
}

MatrixS<-1, -1> coherent_point_drift_rigid::transformed(const MatrixS<-1, -1> &Y){
    return (s * Y * R.transpose()).rowwise() + t.transpose();
}

}