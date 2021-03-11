//
// Created by alex on 08.03.21.
//

#ifndef BCG_GRAPHICS_BCG_ALIGNED_BOX_STATS_H
#define BCG_GRAPHICS_BCG_ALIGNED_BOX_STATS_H

#include "bcg_aligned_box.h"

namespace bcg{

enum class aabb_stats_type{
    percentage_diagonal,
    dinge_heuristic,
    max_side_length,
    min_side_length,
    diagonal,
    volume,
    density,
    __last__
};

std::vector<std::string> aabb_stats_names();

struct aabb_stats{
    static bcg_scalar_t percentage_diagonal(const aligned_box3 &aabb, bcg_scalar_t percent);

    static bcg_scalar_t dinge_heuristic(const aligned_box3 &aabb, size_t num_points_inside);

    static bcg_scalar_t max_side_length(const aligned_box3 &aabb);

    static bcg_scalar_t min_side_length(const aligned_box3 &aabb);

    static bcg_scalar_t diagonal(const aligned_box3 &aabb);

    static bcg_scalar_t volume(const aligned_box3 &aabb);

    static bcg_scalar_t density(const aligned_box3 &aabb, size_t num_points_inside);
};

}

#endif //BCG_GRAPHICS_BCG_ALIGNED_BOX_STATS_H
