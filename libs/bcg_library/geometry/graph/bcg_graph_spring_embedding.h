//
// Created by alex on 11.05.21.
//

#ifndef BCG_GRAPHICS_BCG_GRAPH_SPRING_EMBEDDING_H
#define BCG_GRAPHICS_BCG_GRAPH_SPRING_EMBEDDING_H

#include "bcg_graph.h"

namespace bcg{

void graph_random_embedding_2D(halfedge_graph &graph);

void graph_random_embedding_3D(halfedge_graph &graph);

struct graph_spring_embedding{
    graph_spring_embedding() = default;

    void iterate(halfedge_graph &graph);

    int num_steps = 1;
    float force_eps = scalar_eps;
    float force_coefficient = 1.0;
    float repulsion_coefficient = 1.0;
    float spring_coefficient = 1.0;
    float target_edge_length = 0.1;
    bool annealing = false;
};

}

#endif //BCG_GRAPHICS_BCG_GRAPH_SPRING_EMBEDDING_H
