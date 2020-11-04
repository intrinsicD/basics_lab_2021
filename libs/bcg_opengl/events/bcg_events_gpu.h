//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_GPU_H
#define BCG_GRAPHICS_BCG_EVENTS_GPU_H

#include <vector>
#include <string>
#include "entt/entt.hpp"

namespace bcg::event::gpu{
struct update_vertex_attributes {
    entt::entity id;
    std::vector<std::string> attributes_names;
};
struct update_edge_attributes {
    entt::entity id;
    std::vector<std::string> attributes_names;
};
struct update_face_attributes {
    entt::entity id;
    std::vector<std::string> attributes_names;
};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_GPU_H
