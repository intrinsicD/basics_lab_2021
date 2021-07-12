//
// Created by alex on 12.07.21.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_OVERLAY_POINT_CLOUD_H
#define BCG_GRAPHICS_BCG_EVENTS_OVERLAY_POINT_CLOUD_H

#include "entt/entt.hpp"

namespace bcg::event::overlay::point_cloud{

struct setup{
    entt::entity id;
};

struct make_vertex_selection{
    entt::entity parent_id;
};

struct make_point_selection{
    entt::entity parent_id;
};

}

#endif //BCG_GRAPHICS_BCG_EVENTS_OVERLAY_POINT_CLOUD_H
