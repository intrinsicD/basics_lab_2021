//
// Created by alex on 19.11.20.
//

#ifndef BCG_GRAPHICS_BCG_ENTITY_HIERARCHY_H
#define BCG_GRAPHICS_BCG_ENTITY_HIERARCHY_H

#include <unordered_map>
#include "entt/entt.hpp"
#include "math/bcg_linalg.h"

namespace bcg{

struct entity_hierarchy{
    Transform accumulated_model = Transform::Identity();
    entt::entity parent = entt::null;
    std::unordered_map<entt::entity, entt::entity> children;
};

}

#endif //BCG_GRAPHICS_BCG_ENTITY_HIERARCHY_H
