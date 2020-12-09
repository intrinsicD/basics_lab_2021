//
// Created by alex on 11.11.20.
//

#ifndef BCG_GRAPHICS_BCG_EVENTS_VECTORFIELD_RENDERER_H
#define BCG_GRAPHICS_BCG_EVENTS_VECTORFIELD_RENDERER_H

#include "entt/entt.hpp"
#include "renderers/bcg_attribute.h"

namespace bcg::event::vectorfield_renderer{
struct enqueue{
    entt::entity id;
};
struct setup_for_rendering{
    entt::entity id;
};
struct set_vertex_vectorfield{
    entt::entity id;
    std::string vectorfield_name;
};
struct set_edge_vectorfield{
    entt::entity id;
    std::string vectorfield_name;
};
struct set_face_vectorfield{
    entt::entity id;
    std::string vectorfield_name;
};
struct set_position_attribute{
    entt::entity id;
    std::string vectorfield_name;
    attribute position;
};
struct set_vector_attribute{
    entt::entity id;
    std::string vectorfield_name;
    attribute vector;
};
struct set_color_attribute{
    entt::entity id;
    std::string vectorfield_name;
    attribute color;
};
}

#endif //BCG_GRAPHICS_BCG_EVENTS_VECTORFIELD_RENDERER_H
