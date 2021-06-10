//
// Created by alex on 29.04.21.
//

#ifndef BCG_GRAPHICS_BCG_HQ_PBR_UFSI_RENDERER_EVENTS_H
#define BCG_GRAPHICS_BCG_HQ_PBR_UFSI_RENDERER_EVENTS_H

#include <string>
#include "entt/entt.hpp"

namespace bcg::event::hq_pbr_ufsi_renderer{
struct enqueue {
    entt::entity id;
};
struct setup_material{
    entt::entity id;
};
struct set_position{
    entt::entity id;
    std::string property_name;
};
struct set_color{
    entt::entity id;
    std::string property_name;
};
struct set_normal{
    entt::entity id;
    std::string property_name;
};
}

#endif //BCG_GRAPHICS_BCG_HQ_PBR_UFSI_RENDERER_EVENTS_H
