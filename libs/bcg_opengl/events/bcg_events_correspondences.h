//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_CORRESPONDENCES_H
#define BCG_GRAPHICS_BCG_EVENTS_CORRESPONDENCES_H

#include "entt/entt.hpp"
#include "math/bcg_math_types.h"

namespace bcg::event::correspondences{

struct estimate{
    entt::entity source_id, target_id;
};

namespace filter{
struct distance{
    entt::entity source_id, target_id;
    bcg_scalar_t threshold;
};

struct normal_angle{
    entt::entity source_id, target_id;
    bcg_scalar_t threshold;
};

}


}
#endif //BCG_GRAPHICS_BCG_EVENTS_CORRESPONDENCES_H
