//
// Created by alex on 21.01.21.
//

#ifndef BCG_GRAPHICS_BCG_SAMPLING_LOCALLY_OPTIMAL_PROJECTION_H
#define BCG_GRAPHICS_BCG_SAMPLING_LOCALLY_OPTIMAL_PROJECTION_H

#include "point_cloud/bcg_point_cloud.h"
#include "kdtree/bcg_kdtree.h"
#include "entt/entt.hpp"

namespace bcg {

enum class LopType {
    lop,
    wlop,
    flop,
    clop,
    __last__
};

std::vector<std::string> lop_method_names();
std::vector<std::string> repulsion_type_names();

struct projection_operator {
    property<VectorS<3>, 3> ref_positions, ref_normals, sampling_positions, old_positions, sampling_normals, projection_forces, projection_forces_tangential, repulsion_forces, repulsion_forces_i, repulsion_forces_ii;
    property<bcg_scalar_t, 1> ref_density, sampling_projection_weights, sampling_repulsion_weights;

    bcg_scalar_t attraction_radius = 1.0f;
    bcg_scalar_t feature_radius = 1.0f;
    bcg_scalar_t repulsion_weight = 0.49f;

    bool tangential_projection_force = false;

    enum RepulsionType{
        standard,
        normal,
        normal_average,
        __last__
    }repulsion_type;

    kdtree_property<bcg_scalar_t> ref_index;

    projection_operator() = default;

    virtual void init(vertex_container &ref_vertices, vertex_container &sampling_vertices, bool use_density_weight, size_t parallel_grain_size = 1024);

    virtual bcg_scalar_t compute_attraction_alpha(size_t i, size_t j) = 0;

    virtual bcg_scalar_t compute_repulsion_beta(size_t i, size_t ii) = 0;

    virtual void compute_step(size_t parallel_grain_size = 1024);
};

struct lop : public projection_operator {
    bcg_scalar_t compute_attraction_alpha(size_t i, size_t j) override;

    bcg_scalar_t compute_repulsion_beta(size_t i, size_t ii) override;
};

struct wlop : public projection_operator {
    bcg_scalar_t compute_attraction_alpha(size_t i, size_t j) override;

    bcg_scalar_t compute_repulsion_beta(size_t i, size_t ii) override;
};

struct flop : public projection_operator {
    bcg_scalar_t compute_attraction_alpha(size_t i, size_t j) override;

    bcg_scalar_t compute_repulsion_beta(size_t i, size_t ii) override;
};

struct clop : public projection_operator {
    property<MatrixS<3, 3>, 1> Sigma_S;
    property<bcg_scalar_t , 1> w_s;
    VectorS<3> sigma_k = {0.11772, 0.03287, 0.01010};
    VectorS<3> w_k = {11.453, 29.886, 97.761};

    void init(vertex_container &ref_vertices, vertex_container &sampling_vertices, bool use_density_weight, size_t parallel_grain_size = 1024) override;

    void compute_step(size_t parallel_grain_size = 1024) override;

    bcg_scalar_t compute_attraction_alpha(size_t i, size_t j) override;

    bcg_scalar_t compute_repulsion_beta(size_t i, size_t ii) override;
};

}

#endif //BCG_GRAPHICS_BCG_SAMPLING_LOCALLY_OPTIMAL_PROJECTION_H
