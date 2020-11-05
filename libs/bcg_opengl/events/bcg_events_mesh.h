//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_MESH_H
#define BCG_GRAPHICS_BCG_EVENTS_MESH_H

#include "entt/entt.hpp"

namespace bcg::event::mesh{
struct setup{
    entt::entity id;
    std::string filename;
};

struct make_triangle{

};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_MESH_H
