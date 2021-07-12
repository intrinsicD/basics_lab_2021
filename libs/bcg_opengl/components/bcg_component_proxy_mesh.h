//
// Created by alex on 08.07.21.
//

#ifndef BCG_GRAPHICS_BCG_COMPONENT_PROXY_MESH_H
#define BCG_GRAPHICS_BCG_COMPONENT_PROXY_MESH_H

#include "entt/entt.hpp"
#include "mesh/bcg_mesh.h"

namespace bcg{

struct proxy_mesh : public graph{
    entt::entity parent_id = entt::null;

    proxy_mesh() = default;

    proxy_mesh(entt::entity parent_id) : parent_id(parent_id) {}
};

}

#endif //BCG_GRAPHICS_BCG_COMPONENT_PROXY_MESH_H
