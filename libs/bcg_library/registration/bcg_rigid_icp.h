//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_RIGID_ICP_H
#define BCG_GRAPHICS_BCG_RIGID_ICP_H

#include "math/vector/bcg_vector.h"
#include "math/bcg_linalg.h"
#include "math/rotations/bcg_rotation_optimal.h"

namespace bcg {

template<int D>
inline VectorS<D> weighted_mean(const MatrixS<-1, D> &data, const VectorS<-1> &weights){
    return weights.transpose() * data / weights.sum();
}

template<int D>
inline MatrixS<D, D> weighted_cov(const MatrixS<-1, D> &source, const MatrixS<-1, D> &target, const VectorS<-1> &weights){
    return target.transpose() * weights.asDiagonal() * source;
}

template<int D>
inline VectorS<D> get_translation(const MatrixS<D, D> &R, const VectorS<D> &source_mean, const VectorS<D> &target_mean){
    return target_mean -  R * source_mean;
}

template<int N, int D>
inline MatrixS<D, D> minimize_point_2_point(const MatrixS<N, D> &source, const MatrixS<N, D> &target, const VectorS<D> &source_mean, const VectorS<D> &target_mean){
    return optimal_rotation<N, D>(source.rowwise() - source_mean.transpose(), target.rowwise() - target_mean.transpose(), true);
}

template<int N, int D>
inline MatrixS<D, D> minimize_point_2_point(const MatrixS<N, D> &source, const MatrixS<N, D> &target, const VectorS<D> &source_mean, const VectorS<D> &target_mean, const VectorS<-1> &weights){
    return optimal_rotation<N, D>(source.rowwise() - source_mean.transpose(), weights.asDiagonal(), target.rowwise() - target_mean.transpose(), true);
}

Transform minimize_point_2_point(const MatrixS<-1, 3> &source, const Transform &source_model, const MatrixS<-1, 3> &target, const Transform &target_model, const VectorS<-1> &weights, size_t parallel_grain_size = 1024);

Transform minimize_point_2_point(const MatrixS<-1, 3> &source, const Transform &source_model, const MatrixS<-1, 3> &target, const Transform &target_model, size_t parallel_grain_size = 1024);

Transform minimize_point_2_plane(const MatrixS<-1, 3> &source, const MatrixS<-1, 3> &target, const MatrixS<-1, 3> &target_normals);

Transform minimize_point_2_plane(const MatrixS<-1, 3> &source, const MatrixS<-1, 3> &target, const MatrixS<-1, 3> &target_normals, const VectorS<-1> &weights);

Transform minimize_point_2_plane(const MatrixS<-1, 3> &source, const Transform &source_model, const MatrixS<-1, 3> &target, const MatrixS<-1, 3> &target_normals, const Transform &target_model, size_t parallel_grain_size = 1024);

Transform minimize_point_2_plane(const MatrixS<-1, 3> &source, const Transform &source_model, const MatrixS<-1, 3> &target, const MatrixS<-1, 3> &target_normals, const Transform &target_model, const VectorS<-1> &weights, size_t parallel_grain_size = 1024);



}

#endif //BCG_GRAPHICS_BCG_RIGID_ICP_H
