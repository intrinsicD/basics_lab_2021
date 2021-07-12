//
// Created by alex on 08.07.21.
//

#ifndef BCG_GRAPHICS_BCG_COMPONENT_PROXY_GRAPH_H
#define BCG_GRAPHICS_BCG_COMPONENT_PROXY_GRAPH_H

#include "entt/entt.hpp"
#include "graph/bcg_graph.h"

namespace bcg{

struct proxy_graph : public graph{
    entt::entity parent_id = entt::null;

    proxy_graph() = default;

    proxy_graph(entt::entity parent_id) : parent_id(parent_id) {}
};

}

#endif //BCG_GRAPHICS_BCG_COMPONENT_PROXY_GRAPH_H
