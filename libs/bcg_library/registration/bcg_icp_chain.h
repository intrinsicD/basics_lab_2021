//
// Created by alex on 24.05.21.
//

#ifndef BCG_GRAPHICS_BCG_ICP_CHAIN_H
#define BCG_GRAPHICS_BCG_ICP_CHAIN_H

#include "bcg_icp_features.h"

namespace bcg{

struct PointsSampling;
struct CorrespondenceEstimation;
struct CorrespondenceFiltering;
struct CorrespondenceWeighting;
struct ErrorMinimization;

struct icp_chain{
    PointsSampling *sampling;
    Features features;
    PointsSampling *resampling;
    CorrespondenceEstimation *estimation;
    CorrespondenceFiltering *filtering;
    CorrespondenceWeighting *weighting;
    ErrorMinimization *minimizer;

    icp_chain() = default;

    void build_features();
    float initial_alignment();
    float align_step();
    float align_iters(unsigned int max_iters);
    float align_convergence(float alignment_eps);
    float align_convergence(float alignment_eps, unsigned int max_iters);

    std::vector<float> errors;
    float alignment_eps;
    unsigned int current_iter, max_iters;

    Transform source_model, target_model, best_transformation;

    MatrixS<-1, D> transform_source_to_world();
    MatrixS<-1, D> transform_target_to_world();

    property<VectorS<3>, 3> source_object_space_points;
    property<VectorS<3>, 3> target_object_space_points;
};

}

#endif //BCG_GRAPHICS_BCG_ICP_CHAIN_H
