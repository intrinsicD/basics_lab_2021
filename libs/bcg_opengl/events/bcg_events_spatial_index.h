//
// Created by alex on 20.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_SPATIAL_INDEX_H
#define BCG_GRAPHICS_BCG_EVENTS_SPATIAL_INDEX_H

#include "entt/entt.hpp"

namespace bcg::event::spatial_index{

struct setup_kdtree{
    entt::entity id;
};

struct setup_octree{
    entt::entity id;
};

struct setup_sampling_octree{
    entt::entity id;
};

struct update_indices{
    entt::entity id;
};

}

#endif //BCG_GRAPHICS_BCG_EVENTS_SPATIAL_INDEX_H
