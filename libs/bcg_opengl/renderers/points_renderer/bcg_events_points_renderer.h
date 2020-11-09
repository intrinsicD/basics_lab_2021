//
// Created by alex on 29.10.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_POINTS_RENDERER_H
#define BCG_GRAPHICS_BCG_EVENTS_POINTS_RENDERER_H

#include "entt/entt.hpp"
#include "renderers/bcg_attribute.h"

namespace bcg::event::points_renderer{
struct enqueue{
    entt::entity id;
};
struct set_material{
    entt::entity id;
};
struct set_position_attribute{
    entt::entity id;
    attribute position;
};
struct set_color_attribute{
    entt::entity id;
    attribute color;
};
struct set_point_size_attribute{
    entt::entity id;
    attribute point_size;
};
}
#endif //BCG_GRAPHICS_BCG_EVENTS_POINTS_RENDERER_H
