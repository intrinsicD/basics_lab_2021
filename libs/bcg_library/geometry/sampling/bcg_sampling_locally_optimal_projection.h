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

struct projection_operator {
    property<VectorS<3>, 3> ref_positions, ref_normals, ref_density, sampling_positions, old_positions, sampling_normals;

    bcg_scalar_t attraction_radius = 1.0f;
    bcg_scalar_t feature_radius = 1.0f;
    bcg_scalar_t repulsion_weight = 0.49f;

    projection_operator() = default;

    virtual void init(vertex_container &ref_vertices, vertex_container &sampling_vertices, bool use_density_weight);

    virtual bcg_scalar_t compute_attraction_alpha(size_t i, size_t j) = 0;

    virtual bcg_scalar_t compute_repulsion_beta(size_t i, size_t ii) = 0;

    void compute_step();
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
    property<MatrixS<3, 3>, 1> covs;
    VectorS<3> sigma_k = {0.11772, 0.03287, 0.01010};
    VectorS<3> w_k = {11.453, 29.886, 97.761};

    bcg_scalar_t compute_attraction_alpha(size_t i, size_t j) override;

    bcg_scalar_t compute_repulsion_beta(size_t i, size_t ii) override;
};

}

#endif //BCG_GRAPHICS_BCG_SAMPLING_LOCALLY_OPTIMAL_PROJECTION_H
