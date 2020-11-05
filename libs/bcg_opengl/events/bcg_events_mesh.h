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
struct make_quad{

};
namespace vertex_normals{
struct uniform{
    entt::entity id;
};
struct area{
    entt::entity id;
};
struct angle{
    entt::entity id;
};
struct area_angle{
    entt::entity id;
};
}
}

#endif //BCG_GRAPHICS_BCG_EVENTS_MESH_H
