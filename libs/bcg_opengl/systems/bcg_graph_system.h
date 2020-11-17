//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_GRAPH_SYSTEM_H
#define BCG_GRAPHICS_BCG_GRAPH_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct graph_system : public system {
    explicit graph_system(viewer_state *state);

    void on_edge_centers(const event::graph::edge::centers &event);
};


}

#endif //BCG_GRAPHICS_BCG_GRAPH_SYSTEM_H
