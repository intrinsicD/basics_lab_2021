//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_TRANSFORM_H
#define BCG_GRAPHICS_BCG_EVENTS_TRANSFORM_H

#include "math/bcg_linalg.h"
#include "entt/entt.hpp"

namespace bcg::event::transform{
struct add{
    entt::entity id;
};
struct translate{
    entt::entity id;
    VectorS<3> trans;
};
struct scale{
    entt::entity id;
    VectorS<3> scales;
};
struct rotate{
    entt::entity id;
    VectorS<4> angle_axis;
};
struct reset{
    entt::entity id;
};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_TRANSFORM_H
