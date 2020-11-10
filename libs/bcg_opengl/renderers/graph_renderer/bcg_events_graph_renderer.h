//
// Created by alex on 10.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_GRAPH_RENDERER_H
#define BCG_GRAPHICS_BCG_EVENTS_GRAPH_RENDERER_H

#include "entt/entt.hpp"
#include "renderers/bcg_attribute.h"

namespace bcg::event::graph_renderer {
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
struct set_color_attribute{
    entt::entity id;
    attribute color;
};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_GRAPH_RENDERER_H
