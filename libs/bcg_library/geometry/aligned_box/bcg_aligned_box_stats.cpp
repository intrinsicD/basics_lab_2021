//
// Created by alex on 08.03.21.
//

#include "bcg_aligned_box_stats.h"

namespace bcg{

std::vector<std::string> aabb_stats_names(){
    std::vector<std::string> names(static_cast<int>(aabb_stats_type::__last__));
    names[static_cast<int>(aabb_stats_type::percentage_diagonal)] = "percentage_diagonal";
    names[static_cast<int>(aabb_stats_type::dinge_heuristic)] = "dinge_heuristic";
    names[static_cast<int>(aabb_stats_type::max_side_length)] = "max_side_length";
    names[static_cast<int>(aabb_stats_type::min_side_length)] = "min_side_length";
    names[static_cast<int>(aabb_stats_type::diagonal)] = "diagonal";
    names[static_cast<int>(aabb_stats_type::volume)] = "volume";
    names[static_cast<int>(aabb_stats_type::density)] = "density";
    return names;
}

bcg_scalar_t aabb_stats::percentage_diagonal(const aligned_box3 &aabb, bcg_scalar_t percent){
    return diagonal(aabb) * percent / bcg_scalar_t(100);
}

bcg_scalar_t aabb_stats::dinge_heuristic(const aligned_box3 &aabb, size_t num_points_inside){
    return diagonal(aabb) * std::sqrt(bcg_scalar_t(20) / bcg_scalar_t(num_points_inside)) / 2;
}

bcg_scalar_t aabb_stats::max_side_length(const aligned_box3 &aabb){
    return (aabb.max - aabb.min).maxCoeff();
}

bcg_scalar_t aabb_stats::min_side_length(const aligned_box3 &aabb){
    return (aabb.max - aabb.min).minCoeff();
}

bcg_scalar_t aabb_stats::diagonal(const aligned_box3 &aabb){
    return aabb.diagonal().norm();
}

bcg_scalar_t aabb_stats::volume(const aligned_box3 &aabb){
    return (aabb.max - aabb.min).prod();
}

bcg_scalar_t aabb_stats::density(const aligned_box3 &aabb, size_t num_points_inside){
    return num_points_inside / volume(aabb);
}


}