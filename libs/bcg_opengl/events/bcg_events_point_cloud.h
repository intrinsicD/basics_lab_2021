//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_POINT_CLOUD_H
#define BCG_GRAPHICS_BCG_EVENTS_POINT_CLOUD_H

#include "entt/entt.hpp"

namespace bcg::event::point_cloud{
struct setup{
    entt::entity id;
    std::string filename;
};

}
#endif //BCG_GRAPHICS_BCG_EVENTS_POINT_CLOUD_H
