//
// Created by alex on 13.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_CURVE_RENDERER_H
#define BCG_GRAPHICS_BCG_EVENTS_CURVE_RENDERER_H

#include "entt/entt.hpp"

namespace bcg::event::curve_renderer {
struct enqueue {
    entt::entity id;
};
struct setup_for_rendering{
    entt::entity id;
};
struct set_position_attribute{
    entt::entity id;
    attribute position;
};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_CURVE_RENDERER_H
