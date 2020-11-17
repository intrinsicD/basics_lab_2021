//
// Created by alex on 16.11.20.
//

#include "bcg_graph_edge_centers.h"
#include "tbb/tbb.h"

namespace bcg {

VectorS<3> edge_center(const halfedge_graph &graph, edge_handle e) {
    return (graph.positions[graph.get_vertex(e, 0)] + graph.positions[graph.get_vertex(e, 1)]) / 2.0;
}

void edge_centers(halfedge_graph &graph, size_t parallel_grain_size) {
    auto positions = graph.edges.get_or_add<VectorS<3>, 3>("e_position");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) graph.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range <uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    positions[i] = edge_center(graph, i);
                }
            }
    );
    positions.set_dirty();
}

}