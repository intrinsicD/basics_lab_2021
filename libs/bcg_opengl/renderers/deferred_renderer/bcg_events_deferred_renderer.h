//
// Created by alex on 15.12.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_DEFERRED_RENDERER_H
#define BCG_GRAPHICS_BCG_EVENTS_DEFERRED_RENDERER_H

#include "entt/entt.hpp"

namespace bcg::event::deferred_renderer{

struct enqueue{
   entt::entity id;
};

struct setup_for_rendering{
    entt::entity id;
};

}


#endif //BCG_GRAPHICS_BCG_EVENTS_DEFERRED_RENDERER_H
