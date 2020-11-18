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
namespace edge{
struct dihedral_angle{
    entt::entity id;
};
}
namespace face{
struct centers{
    entt::entity id;
};
struct normals{
    entt::entity id;
};
}


namespace vertex_normals{
enum Type{
    UNIFORM,
    AREA,
    ANGLE,
    AREAANGLE
};

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
