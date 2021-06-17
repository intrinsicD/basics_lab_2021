//
// Created by alex on 17.06.21.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_OBJECT_SPACE_H
#define BCG_GRAPHICS_BCG_EVENTS_OBJECT_SPACE_H

#include "entt/entt.hpp"
#include "math/vector/bcg_vector.h"
#include "math/bcg_linalg.h"

namespace bcg::event::object_space {
struct add_component_object_space_transform {
    entt::entity id;
};
struct set_component_object_space_transform {
    entt::entity id;
    Transform transform;
};
struct translate_aabb_center_to_origin{
    entt::entity id;
};
struct uniform_scale_to_unit_cube{
    entt::entity id;
};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_OBJECT_SPACE_H
