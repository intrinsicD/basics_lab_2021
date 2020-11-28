//
// Created by alex on 28.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GRAPH_EDGE_LENGTHS_H
#define BCG_GRAPHICS_BCG_GRAPH_EDGE_LENGTHS_H

#include "bcg_graph.h"

namespace bcg {

bcg_scalar_t edge_length(const halfedge_graph &graph, edge_handle e);

void edge_lengths(halfedge_graph &graph, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_GRAPH_EDGE_LENGTHS_H
