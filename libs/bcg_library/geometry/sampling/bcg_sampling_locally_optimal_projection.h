//
// Created by alex on 21.01.21.
//

#ifndef BCG_GRAPHICS_BCG_SAMPLING_LOCALLY_OPTIMAL_PROJECTION_H
#define BCG_GRAPHICS_BCG_SAMPLING_LOCALLY_OPTIMAL_PROJECTION_H

#include "point_cloud/bcg_point_cloud.h"
#include "kdtree/bcg_kdtree.h"

namespace bcg {

enum LopType{
    lop,
    wlop,
    flop,
    clop,
    custom,
    __last__
};

std::vector<std::string> method_names();

struct projection_operator{
    enum Type{
        lop,
        wlop,
        flop,
        clop
    };

    Matrix<-1, -1> ALPHA, BETA;
    bcg_scalar_t attraction_radius, repulsion_weight;
    std::function<bcg_scalar_t(bcg_scalar_t distance, bcg_scalar_t support_radius)> attraction_callback;
    std::function<bcg_scalar_t(bcg_scalar_t distance)> repulsion_callback, repulsion_derivative_callback;
    property<VectorS<3>, 3> ref_positions, ref_normals, ref_density, sampling_positions, samplling_normals;

    projection_operator() = default;

    void init(Vertices &ref_positions, Vertices &sampling_positions);

    void compute_step();
};

bcg_scalar_t compute_attraction_weight_point(bcg_scalar_t distance, bcg_scalar_t support_radius) const;

bcg_scalar_t compute_attraction_weight_plane(bcg_scalar_t distance, bcg_scalar_t support_radius) const;

bcg_scalar_t compute_attraction_weight_density(bcg_scalar_t distance, bcg_scalar_t support_radius) const;

bcg_scalar_t compute_repulsion_weight_lop(bcg_scalar_t distance) const;

bcg_scalar_t compute_repulsion_weight_wlop(bcg_scalar_t distance) const;

bcg_scalar_t compute_repulsion_weight_lop_derivative(bcg_scalar_t distance) const;

bcg_scalar_t compute_repulsion_weight_wlop_derivative(bcg_scalar_t distance) const;

void local_density_estimation(Vertices &ref_vertices, bcg_scalar_t attraction_radius, const kdtree_property<bcg_scalar_t> &ref_index, size_t parallel_grain_size = 1024);

void locally_optimal_projection_step(Vertices &sample_vertices, const Vertices &ref_vertices, bcg_scalar_t attraction_radius, bcg_scalar_t repulsion_weight, const kdtree_property<bcg_scalar_t> &ref_index, size_t parallel_grain_size = 1024);

void locally_optimal_projection_step(Vertices &sample_vertices, const Vertices &ref_vertices, bcg_scalar_t attraction_radius, bcg_scalar_t repulsion_weight, const kdtree_property<bcg_scalar_t> &ref_index, size_t parallel_grain_size = 1024);

void locally_weighted_optimal_projection_step(Vertices &sample_vertices, const Vertices &ref_vertices, bcg_scalar_t attraction_radius, bcg_scalar_t repulsion_weight, const kdtree_property<bcg_scalar_t> &ref_index, size_t parallel_grain_size = 1024);

void locally_feature_preserving_optimal_projection_step(Vertices &sample_vertices, const Vertices &ref_vertices, bcg_scalar_t attraction_radius, bcg_scalar_t repulsion_weight, const kdtree_property<bcg_scalar_t> &ref_index, size_t parallel_grain_size = 1024);

void locally_continuous_optimal_projection_step(Vertices &sample_vertices, const Vertices &ref_vertices, bcg_scalar_t attraction_radius, bcg_scalar_t repulsion_weight, const kdtree_property<bcg_scalar_t> &ref_index, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_SAMPLING_LOCALLY_OPTIMAL_PROJECTION_H
