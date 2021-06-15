//
// Created by alex on 31.03.21.
//

#ifndef BCG_GRAPHICS_BCG_GRAPH_DIJKSTRA_H
#define BCG_GRAPHICS_BCG_GRAPH_DIJKSTRA_H

#include "bcg_graph.h"

namespace bcg {

struct geometric_path {
    bcg_scalar_t length;
    std::vector<vertex_handle> vertices;
    std::vector<edge_handle> edges;
};

void graph_shortest_paths_from(halfedge_graph &graph, vertex_handle v_start);

void graph_shortest_paths_from(halfedge_graph &graph, vertex_handle v_start, property<bcg_scalar_t, 1> heuristic, property<VectorS<3>, 3> guide_vectorfield);

geometric_path graph_shortest_path_between(halfedge_graph &graph, vertex_handle v_start, vertex_handle v_end);

geometric_path graph_shortest_path_between(halfedge_graph &graph, vertex_handle v_start, vertex_handle v_end, property<bcg_scalar_t, 1> heuristic, property<VectorS<3>, 3> guide_vectorfield);

}

#endif //BCG_GRAPHICS_BCG_GRAPH_DIJKSTRA_H
