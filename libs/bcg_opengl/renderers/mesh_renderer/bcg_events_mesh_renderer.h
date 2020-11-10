//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_MESH_RENDERER_H
#define BCG_GRAPHICS_BCG_EVENTS_MESH_RENDERER_H

#include "entt/entt.hpp"
#include "renderers/bcg_attribute.h"

namespace bcg::event::mesh_renderer {
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
struct set_normal_attribute{
    entt::entity id;
    attribute normal;
};
struct set_color_attribute{
    entt::entity id;
    attribute color;
};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_MESH_RENDERER_H
