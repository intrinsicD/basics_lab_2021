//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GRAPH_EDGE_CENTERS_H
#define BCG_GRAPHICS_BCG_GRAPH_EDGE_CENTERS_H

#include "bcg_graph.h"

namespace bcg {

VectorS<3> edge_center(const halfedge_graph &graph, edge_handle e);

void edge_centers(halfedge_graph &graph, size_t parallel_grain_size = 1024);

}

#endif //BCG_GRAPHICS_BCG_GRAPH_EDGE_CENTERS_H
