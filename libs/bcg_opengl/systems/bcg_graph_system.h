//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_GRAPH_SYSTEM_H
#define BCG_GRAPHICS_BCG_GRAPH_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct graph_system : public system {
    explicit graph_system(viewer_state *state);

    void on_setup(const event::graph::setup &event);

    void on_vertex_pca(const event::graph::vertex::pca &event);
};


}

#endif //BCG_GRAPHICS_BCG_GRAPH_SYSTEM_H
