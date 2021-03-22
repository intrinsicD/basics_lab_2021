//
// Created by alex on 20.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_SPATIAL_INDEX_H
#define BCG_GRAPHICS_BCG_EVENTS_SPATIAL_INDEX_H

#include "entt/entt.hpp"
#include "sampling/bcg_sampling_octree.h"

namespace bcg::event::spatial_index{

struct setup_kdtree{
    entt::entity id;
};

struct setup_octree{
    entt::entity id;
    size_t leaf_size = 10;
    int max_depth = 21;
};

struct setup_sampling_octree{
    entt::entity id;
    sampling_octree::SamplingType type;
    std::vector<size_t> indices;
    size_t leaf_size = 10;
    int max_depth = 21;
};

struct update_indices{
    entt::entity id;
};

}

#endif //BCG_GRAPHICS_BCG_EVENTS_SPATIAL_INDEX_H
