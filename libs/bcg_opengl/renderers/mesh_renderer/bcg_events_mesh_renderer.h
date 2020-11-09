//
// Created by alex on 05.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_MESH_RENDERER_H
#define BCG_GRAPHICS_BCG_EVENTS_MESH_RENDERER_H

#include "entt/entt.hpp"

namespace bcg::event::mesh_renderer {
struct enqueue {
    entt::entity id;
};
struct set_material{
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
