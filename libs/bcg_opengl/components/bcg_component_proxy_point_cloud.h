//
// Created by alex on 08.07.21.
//

#ifndef BCG_GRAPHICS_BCG_COMPONENT_PROXY_POINT_CLOUD_H
#define BCG_GRAPHICS_BCG_COMPONENT_PROXY_POINT_CLOUD_H

#include "entt/entt.hpp"
#include "point_cloud/bcg_point_cloud.h"

namespace bcg{

struct proxy_point_cloud : public point_cloud{
    entt::entity parent_id = entt::null;

    proxy_point_cloud() = default;

    proxy_point_cloud(entt::entity parent_id) : parent_id(parent_id) {}
};

}

#endif //BCG_GRAPHICS_BCG_COMPONENT_PROXY_POINT_CLOUD_H
