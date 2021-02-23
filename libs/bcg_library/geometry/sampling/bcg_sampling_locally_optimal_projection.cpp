//
// Created by alex on 21.01.21.
//

#include "bcg_sampling_locally_optimal_projection.h"
#include "math/bcg_kernel_functions.h"
#include "bcg_property_map_eigen.h"
#include "math/vector/bcg_vector_map_eigen.h"
#include "tbb/tbb.h"

namespace bcg {

std::vector<std::string> lop_method_names() {
    std::vector<std::string> names(static_cast<int>(LopType::__last__));
    names[static_cast<int>(LopType::lop)] = "lop";
    names[static_cast<int>(LopType::wlop)] = "wlop";
    names[static_cast<int>(LopType::flop)] = "flop";
    names[static_cast<int>(LopType::clop)] = "clop";
    return names;
}

std::vector<std::string> repulsion_type_names(){
    std::vector<std::string> names(static_cast<int>(projection_operator::RepulsionType::__last__));
    names[static_cast<int>(projection_operator::RepulsionType::standard)] = "standard";
    names[static_cast<int>(projection_operator::RepulsionType::normal)] = "normal";
    names[static_cast<int>(projection_operator::RepulsionType::normal_average)] = "normal_average";
    return names;
}

void projection_operator::init(vertex_container &ref_vertices, vertex_container &sampling_vertices,
                               bool use_density_weight, size_t parallel_grain_size) {
    ref_positions = ref_vertices.get<VectorS<3>, 3>("v_position");
    ref_normals = ref_vertices.get<VectorS<3>, 3>("v_normal");
    ref_density = ref_vertices.get_or_add<bcg_scalar_t, 1>("v_lop_density", 1.0f);
    sampling_positions = sampling_vertices.get<VectorS<3>, 3>("v_position");
    old_positions = sampling_vertices.get_or_add<VectorS<3>, 3>("v_old_position");
    sampling_normals = sampling_vertices.get_or_add<VectorS<3>, 3>("v_normal");
    sampling_projection_weights = sampling_vertices.get_or_add<bcg_scalar_t , 1>("v_projection_weight");
    sampling_repulsion_weights = sampling_vertices.get_or_add<bcg_scalar_t , 1>("v_sampling_repulsion_weight");
    projection_forces = sampling_vertices.get_or_add<VectorS<3>, 3>("v_projection_force");
    projection_forces_tangential = sampling_vertices.get_or_add<VectorS<3>, 3>("v_projection_force_tangential");
    repulsion_forces = sampling_vertices.get_or_add<VectorS<3>, 3>("v_repulsion_force");
    repulsion_forces_i = sampling_vertices.get_or_add<VectorS<3>, 3>("v_repulsion_force_i");
    repulsion_forces_ii = sampling_vertices.get_or_add<VectorS<3>, 3>("v_repulsion_force_ii");
    ref_index.build(ref_positions);

    if (use_density_weight) {
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) ref_positions.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);
                        ref_density[v] = 0;
                        //auto result = ref_index.query_radius(ref_positions[v], attraction_radius);
                        auto result = ref_index.query_knn(ref_positions[i], 12);
                        for (size_t j = 0; j < result.indices.size(); ++j) {
                            ref_density[v] += std::exp(-result.distances[j] * result.distances[j] /
                                                       (attraction_radius * attraction_radius / 16));
                        }
                    }
                }
        );
    } else {
        ref_density.reset(1.0f);
    }

    old_positions.vector() = sampling_positions.vector();
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) old_positions.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);

                    auto result = ref_index.query_radius(old_positions[i], attraction_radius);
                    if (attraction_radius < scalar_eps || result.indices.size() <= 1) {
                        result = ref_index.query_knn(old_positions[i], 12);
                    }

                    sampling_normals[v].setZero();
                    for (size_t j = 0; j < result.indices.size(); ++j) {
                        if (result.distances[j] < scalar_eps) continue;
                        auto idx = result.indices[j];
                        bcg_scalar_t weight = compute_attraction_alpha(i, idx) / ref_density[idx];
                        sampling_normals[v] += ref_normals[idx] * weight;
                    }

                    sampling_normals[v].normalize();
                }
            }
    );
    sampling_normals.set_dirty();
    if(tangential_projection_force){
        tangential_projection_force = false;
        compute_step(parallel_grain_size);
        tangential_projection_force = true;
    }
}

void projection_operator::compute_step(size_t parallel_grain_size) {
    old_positions.vector() = sampling_positions.vector();
    auto old_normals = sampling_normals.vector();

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) old_positions.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);

                    auto result = ref_index.query_radius(old_positions[i], attraction_radius);
                    if (attraction_radius < scalar_eps || result.indices.size() <= 1) {
                        result = ref_index.query_knn(old_positions[i], 12);
                        attraction_radius = Map(result.distances).mean();
                    }

                    bcg_scalar_t sum_alpha = 0;

                    sampling_normals[v].setZero();
                    projection_forces[v].setZero();
                    projection_forces_tangential[v].setZero();
                    for (size_t j = 0; j < result.indices.size(); ++j) {
                        if (result.distances[j] < scalar_eps) continue;
                        auto idx = result.indices[j];
                        bcg_scalar_t weight = compute_attraction_alpha(i, idx) / ref_density[idx];
                        VectorS<3> diff = ref_positions[idx] - old_positions[v];
                        projection_forces[v] += diff * weight;
                        VectorS<3> diff_tangential = old_normals[v.idx].dot(diff) * old_normals[v.idx];
                        if(diff_tangential.norm() > diff.norm()){
                            diff_tangential = diff_tangential.normalized() * diff.norm();
                        }
                        projection_forces_tangential[v] += diff_tangential * weight;
                        sampling_normals[v] += ref_normals[idx] * weight;
                        sum_alpha += weight;
                    }
                    sampling_projection_weights[v] = sum_alpha;
                    sampling_normals[v].normalize();
                    projection_forces[v] /= sum_alpha;
                    projection_forces_tangential[v] /= sum_alpha;

                    bcg_scalar_t sum_beta = 0;
                    repulsion_forces[v].setZero();
                    repulsion_forces_i[v].setZero();
                    repulsion_forces_ii[v].setZero();

                    for (size_t ii = 0; ii < old_positions.size(); ++ii) {
                        if (ii == i) continue;
                        bcg_scalar_t weight = compute_repulsion_beta(v.idx, ii);
                        VectorS<3> diff = old_positions[v] - old_positions[ii];
                        repulsion_forces[v] += diff * weight;
                        repulsion_forces_i[v] += (diff - sampling_normals[v].dot(diff) * sampling_normals[v]) * weight;
                        repulsion_forces_ii[v] +=
                                (diff - sampling_normals[ii].dot(diff) * sampling_normals[ii]) * weight;
                        sum_beta += weight;
                    }
                    sampling_repulsion_weights[v] = sum_beta;
                    if (!std::isnan(sum_alpha) && !std::isinf(sum_alpha) && sum_alpha > scalar_eps) {
                        if(tangential_projection_force){
                            sampling_positions[v] += projection_forces_tangential[v];
                        }else{
                            sampling_positions[v] += projection_forces[v];
                        }
                    }

                    repulsion_forces[v] *= repulsion_weight / sum_beta;
                    repulsion_forces_i[v] *= repulsion_weight / sum_beta;
                    repulsion_forces_ii[v] *= repulsion_weight / sum_beta;

                    if (!std::isnan(sum_beta) && !std::isinf(sum_beta) && sum_beta > scalar_eps) {
                        switch(repulsion_type){
                            case RepulsionType::standard : {
                                sampling_positions[v] += repulsion_forces[v];
                                break;
                            }
                            case RepulsionType::normal : {
                                sampling_positions[v] += repulsion_forces_i[v];
                                break;
                            }
                            case RepulsionType::normal_average : {
                                sampling_positions[v] += repulsion_forces_ii[v];
                                break;
                            }
                            case RepulsionType::__last__ : {
                                break;
                            }
                        }

                    }
                }
            }
    );
    sampling_positions.set_dirty();
    sampling_normals.set_dirty();
    projection_forces.set_dirty();
    projection_forces_tangential.set_dirty();
    repulsion_forces.set_dirty();
    repulsion_forces_i.set_dirty();
    repulsion_forces_ii.set_dirty();
    sampling_repulsion_weights.set_dirty();
    sampling_projection_weights.set_dirty();
}


bcg_scalar_t lop::compute_attraction_alpha(size_t i, size_t j) {
    bcg_scalar_t distance = (old_positions[i] - ref_positions[j]).norm();
    bcg_scalar_t normalizer = std::max(distance, scalar_eps);
    return std::exp(-distance * distance / (attraction_radius * attraction_radius / 16)) / (normalizer);
}

bcg_scalar_t lop::compute_repulsion_beta(size_t i, size_t ii) {
    bcg_scalar_t distance = (old_positions[i] - old_positions[ii]).norm();
    bcg_scalar_t normalizer = std::max(distance, scalar_eps);
    return std::exp(-distance * distance / (attraction_radius * attraction_radius / 16)) /
           (normalizer * std::pow(normalizer, 4));
}

bcg_scalar_t wlop::compute_attraction_alpha(size_t i, size_t j) {
    bcg_scalar_t distance = (old_positions[i] - ref_positions[j]).norm();
    bcg_scalar_t normalizer = std::max(distance, scalar_eps);
    return std::exp(-distance * distance / (attraction_radius * attraction_radius / 16)) / (normalizer);
}

bcg_scalar_t wlop::compute_repulsion_beta(size_t i, size_t ii) {
    bcg_scalar_t distance = (old_positions[i] - old_positions[ii]).norm();
    bcg_scalar_t normalizer = std::max(distance, scalar_eps);
    return std::exp(-distance * distance / (attraction_radius * attraction_radius / 16)) / (normalizer);
}

bcg_scalar_t flop::compute_attraction_alpha(size_t i, size_t j) {
    VectorS<3> diff = old_positions[i] - ref_positions[j];
    bcg_scalar_t distance = (diff).norm();
    bcg_scalar_t distance_squared = distance * distance;
    bcg_scalar_t distance_plane = ref_normals[j].dot(diff);
    bcg_scalar_t normalizer = std::max(distance, scalar_eps);
    return std::exp(-distance_squared / (attraction_radius * attraction_radius / 16)) *
           std::exp(-distance_plane * distance_plane / (2 * feature_radius * feature_radius)) / (normalizer);
}

bcg_scalar_t flop::compute_repulsion_beta(size_t i, size_t ii) {
    bcg_scalar_t distance = (old_positions[i] - old_positions[ii]).norm();
    bcg_scalar_t normalizer = std::max(distance, scalar_eps);
    return std::exp(-distance * distance / (attraction_radius * attraction_radius / 16)) / (normalizer);
}

void clop::init(vertex_container &ref_vertices, vertex_container &sampling_vertices, bool use_density_weight, size_t parallel_grain_size) {

}

void clop::compute_step(size_t parallel_grain_size){

}

bcg_scalar_t clop::compute_attraction_alpha(size_t i, size_t j) {

}

bcg_scalar_t clop::compute_repulsion_beta(size_t i, size_t ii) {

}

}