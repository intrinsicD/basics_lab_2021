//
// Created by alex on 21.01.21.
//

#include "bcg_sampling_locally_optimal_projection.h"
#include "math/bcg_kernel_functions.h"

namespace bcg {

std::vector<std::string> lop_method_names() {
    std::vector<std::string> names(static_cast<int>(LopType::__last__));
    names[static_cast<int>(LopType::lop)] = "lop";
    names[static_cast<int>(LopType::wlop)] = "wlop";
    names[static_cast<int>(LopType::flop)] = "flop";
    names[static_cast<int>(LopType::clop)] = "clop";
    return names;
}

void projection_operator::init(vertex_container &ref_vertices, vertex_container &sampling_vertices,
                               bool use_density_weight) {
    ref_positions = ref_vertices.get<VectorS<3>, 3>("v_position");
    ref_normals = ref_vertices.get<VectorS<3>, 3>("v_normal");
    ref_density = ref_vertices.get_or_add<VectorS<3>, 3>("v_lop_density", 1.0f);
    sampling_positions = sampling_vertices.get<VectorS<3>, 3>("v_position");
    old_positions = sampling_vertices.get_or_add<VectorS<3>, 3>("v_old_position");
    sampling_normals = sampling_vertices.get<VectorS<3>, 3>("v_normal");

    if (use_density_weight) {
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) ref_positions.size(), parallel_grain_size),
                [&](const tbb::blocked_range <uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);
                        ref_density[v] = 0;
                        auto result = ref_index.query_radius(ref_positions[v], attraction_radius);
                        for (size_t j = 0; j < result.indices.size(); ++j) {
                            ref_density[v] += attraction_kernel(result.distances[j], attraction_radius);
                        }
                    }
                }
        );
    } else {
        ref_density.reset(1.0f);
    }
}

void projection_operator::compute_step() {
    old_positions.vector() = sampling_positions.vector();

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) old_positions.size(), parallel_grain_size),
            [&](const tbb::blocked_range <uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);

                    auto result = ref_index.query_radius(old_positions[i], attraction_radius);
                    bcg_scalar_t sum_alpha = 0;
                    bcg_scalar_t normal_projection = 0;
                    bcg_scalar_t density = 1.0;
                    VectorS<3> projection = VectorS<3>::Zero();
                    for (size_t j = 0; j < result.indices.size(); ++j) {
                        bcg_scalar_t weight = compute_attraction_alpha(i, j) / ref_density[j];
                        projection += ref_positions[j] * weight;
                        sum_alpha += weight;
                    }

                    VectorS<3> rejection = VectorS<3>::Zero();
                    bcg_scalar_t sum_beta = 0;
                    for (size_t ii = 0; ii < old_positions.size(); ++ii) {
                        bcg_scalar_t weight = compute_repulsion_beta(i, ii);
                        rejection += (old_positions[ii] - old_positions[i]) * weight;
                        sum_beta += weight;
                    }

                    sampling_positions[v] = projection / sum_alpha + repulsion_weight * rejection / sum_beta;
                }
            }
    );
    sampling_positions.set_dirty();
}


bcg_scalar_t lop::compute_attraction_alpha(size_t i, size_t j) {
    bcg_scalar_t distance = (old_positions[i] - ref_positions[j]).norm();
    return std::exp(-distance * distance / (attraction_radius * attraction_radius / 16)) / distance;
}

bcg_scalar_t lop::compute_repulsion_beta(size_t i, size_t ii) {
    bcg_scalar_t distance = (old_positions[i] - old_positions[ii]).norm();
    return -std::exp(-distance * distance / (attraction_radius * attraction_radius / 16)) /
           (distance * std::pow(distance, 4));
}

bcg_scalar_t wlop::compute_attraction_alpha(size_t i, size_t j) {
    bcg_scalar_t distance = (old_positions[i] - ref_positions[j]).norm();
    return std::exp(-distance * distance / (attraction_radius * attraction_radius / 16)) / distance;
}

bcg_scalar_t wlop::compute_repulsion_beta(size_t i, size_t ii) {
    bcg_scalar_t distance = (old_positions[i] - old_positions[ii]).norm();
    return -std::exp(-distance * distance / (attraction_radius * attraction_radius / 16)) / distance;
}

bcg_scalar_t flop::compute_attraction_alpha(size_t i, size_t j) {
    bcg_scalar_t distance = (old_positions[i] - ref_positions[j]).norm();
    bcg_scalar_t distance_squared = distance * distance;
    return std::exp(-distance_squared / (attraction_radius * attraction_radius / 16)) *
           std::exp(-distance_squared / (2 * feature_radius * feature_radius)) / distance;
}

bcg_scalar_t flop::compute_repulsion_beta(size_t i, size_t ii) {
    bcg_scalar_t distance = (old_positions[i] - old_positions[ii]).norm();
    return -std::exp(-distance * distance / (attraction_radius * attraction_radius / 16)) / distance;
}

bcg_scalar_t clop::compute_attraction_alpha(size_t i, size_t j) {

}

bcg_scalar_t clop::compute_repulsion_beta(size_t i, size_t ii) {

}

}