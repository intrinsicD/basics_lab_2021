//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_GPU_H
#define BCG_GRAPHICS_BCG_EVENTS_GPU_H

#include <vector>
#include <string>
#include "entt/entt.hpp"
#include "bcg_opengl/renderers/bcg_attribute.h"

namespace bcg::event::gpu{
struct update_vertex_attributes {
    entt::entity id;
    std::vector<attribute> attributes;
};
struct update_edge_attributes {
    entt::entity id;
    std::vector<attribute> attributes;
};
struct update_face_attributes {
    entt::entity id;
    std::vector<attribute> attributes;
};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_GPU_H
