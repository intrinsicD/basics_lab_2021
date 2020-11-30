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

    void on_vertex_pca_svd(const event::graph::vertex::pca::svd &event);

    void on_vertex_pca_weighted_svd(const event::graph::vertex::pca::weighted_svd &event);

    void on_vertex_pca_eig(const event::graph::vertex::pca::eig &event);

    void on_vertex_pca_weighted_eig(const event::graph::vertex::pca::weighted_eig &event);
};


}

#endif //BCG_GRAPHICS_BCG_GRAPH_SYSTEM_H
