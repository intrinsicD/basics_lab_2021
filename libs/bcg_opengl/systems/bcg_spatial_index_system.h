//, index.leaf_size, index.max_depth
// Created by alex on 20.11.20.
//

#ifndef BCG_GRAPHICS_BCG_SPATIAL_INDEX_SYSTEM_H
#define BCG_GRAPHICS_BCG_SPATIAL_INDEX_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct spatial_index_system : public system {
    explicit spatial_index_system(viewer_state *state);

    void on_setup_kdtree(const event::spatial_index::setup_kdtree &event);

    void on_setup_octree(const event::spatial_index::setup_octree &event);

    void on_setup_sampling_octree(const event::spatial_index::setup_sampling_octree &event);

    void on_update_indices(const event::spatial_index::update_indices &event);
};

}

#endif //BCG_GRAPHICS_BCG_SPATIAL_INDEX_SYSTEM_H
