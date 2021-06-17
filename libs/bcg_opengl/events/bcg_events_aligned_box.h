//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_ALIGNED_BOX_H
#define BCG_GRAPHICS_BCG_EVENTS_ALIGNED_BOX_H

#include "entt/entt.hpp"
#include "aligned_box/bcg_aligned_box.h"

namespace bcg::event::aligned_box{
struct add{
    entt::entity id;
};
struct set{
    entt::entity id;
    aligned_box3 aabb;
};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_ALIGNED_BOX_H
