//
// Created by alex on 24.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MATRIX_COVARIANCE_H
#define BCG_GRAPHICS_BCG_MATRIX_COVARIANCE_H

#include "bcg_matrix.h"
#include "math/vector/bcg_vector.h"

namespace bcg {

template<int M, int D>
inline MatrixS <D, D> covariance(const MatrixS <M, D> &mat1, const MatrixS <M, D> &mat2) {
    return mat1.transpose() * mat2 / (mat1.rows() - 1);
}

template<int M, int D>
inline MatrixS <D, D> covariance(const MatrixS <M, D> &mat1, const VectorS <M> &weights, const MatrixS <M, D> &mat2) {
    return mat1.transpose() * weights.asDiagonal() * mat2 / weights.sum();
}

template<int D>
struct CovarianceMatrix {
    void clear() {
        sum_weights = 0;
        count = 0;
        matrix.setZero();
    }

    void append_outer_product(const MatrixS<D, D> &op){
        append_outer_product(op, 1.0);
    }

    void append_outer_product(const MatrixS<D, D> &op, bcg_scalar_t weight){
        if(D == -1){
            clear(op.rows());
        }
        matrix += weight * op;
        sum_weights += weight;
        ++count;
    }

    void append_points(const VectorS <D> &p, const VectorS <D> &q) {
        append_points(p, q, 1);
    }

    void append_points(const VectorS <D> &p, const VectorS <D> &q, bcg_scalar_t weight) {
        append_outer_product(p * q.transpose(), weight);
    }

    void divide_by_sum_weights(){
        matrix /= sum_weights;
    }

    void build(const MatrixS<-1, D> &P, const MatrixS<-1, D> &Q){
        operator()(P, Q);
    }

    void build_unbiased(const MatrixS<-1, D> &P, const MatrixS<-1, D> &Q){
        clear(P.rows());
        append_outer_product(P.transpose() * Q);
        count = P.rows();
        sum_weights = count - 1;
        divide_by_sum_weights();
    }

    void build(const MatrixS<-1, D> &P, const MatrixS<-1, D> &Q, const VectorS<-1> &weights){
        operator()(P, Q, weights);
    }

    MatrixS<D, D> operator()(const MatrixS<-1, D> &P, const MatrixS<-1, D> &Q){
        clear(P.rows());
        append_outer_product(P.transpose() * Q);
        count = P.rows();
        sum_weights = count - 1;
        divide_by_sum_weights();
        return matrix;
    }

    MatrixS<D, D> operator()(const MatrixS<-1, D> &P, const MatrixS<-1, D> &Q, const VectorS<-1> &weights){
        clear(P.rows());
        append_outer_product(P.transpose() * weights.asDiagonal() * Q);
        count = P.rows();
        sum_weights = weights.sum();
        divide_by_sum_weights();
        return matrix;
    }

    MatrixS <D, D> matrix;
    bcg_scalar_t sum_weights;
    unsigned int count;
private:

    void clear(int dims) {
        sum_weights = 0;
        count = 0;
        matrix = MatrixS<D, D>::Zero(dims, dims);
    }
};


}

#endif //BCG_GRAPHICS_BCG_MATRIX_COVARIANCE_H
