//
// Created by alex on 19.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_HIERARCHY_H
#define BCG_GRAPHICS_BCG_EVENTS_HIERARCHY_H

#include "entt/entt.hpp"

namespace bcg::event::hierarchy{
struct set_parent{
    entt::entity id;
    entt::entity parent_id;
};
struct add_child{
    entt::entity id;
    entt::entity child_id;
};
struct remove_child{
    entt::entity id;
    entt::entity child_id;
};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_HIERARCHY_H
