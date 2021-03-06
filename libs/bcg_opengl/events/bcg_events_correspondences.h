//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_CORRESPONDENCES_H
#define BCG_GRAPHICS_BCG_EVENTS_CORRESPONDENCES_H

#include "entt/entt.hpp"
#include "math/bcg_math_types.h"
#include "bcg_events_filter.h"

namespace bcg::event::correspondences{


struct estimate{
    entt::entity source_id, target_id;
};

struct estimate_and_filter{
    entt::entity source_id, target_id;
    bool filter_distances, filter_normal_angle;
    bcg_scalar_t distance_threshold, normal_threshold;
};

struct filter : bcg::event::filter::correspondences{

};


}
#endif //BCG_GRAPHICS_BCG_EVENTS_CORRESPONDENCES_H
