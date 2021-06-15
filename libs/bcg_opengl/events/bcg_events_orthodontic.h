//
// Created by alex on 15.06.21.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_ORTHODONTIC_H
#define BCG_GRAPHICS_BCG_EVENTS_ORTHODONTIC_H

#include "entt/entt.hpp"

namespace bcg {

namespace event::orthodontic {

struct set_jaw_component{
    entt::entity id;
};

struct set_tooth_component{
    entt::entity id;
};

}

}

#endif //BCG_GRAPHICS_BCG_EVENTS_ORTHODONTIC_H
