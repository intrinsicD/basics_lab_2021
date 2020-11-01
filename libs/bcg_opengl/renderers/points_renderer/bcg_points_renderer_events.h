//
// Created by alex on 29.10.20.
//

#ifndef BCG_GRAPHICS_BCG_POINTS_RENDERER_EVENTS_H
#define BCG_GRAPHICS_BCG_POINTS_RENDERER_EVENTS_H

#include "entt/entt.hpp"

namespace bcg::event::points_renderer{
struct enqueue{
    entt::entity id;
};
}
#endif //BCG_GRAPHICS_BCG_POINTS_RENDERER_EVENTS_H
