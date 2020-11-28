//
// Created by alex on 28.11.20.
//

#include "bcg_graph_edge_lengths.h"
#include "tbb/tbb.h"

namespace bcg {

bcg_scalar_t edge_length(const halfedge_graph &graph, edge_handle e) {
    return (graph.positions[graph.get_vertex(e, 1)] - graph.positions[graph.get_vertex(e, 0)]).norm();
}

void edge_lengths(halfedge_graph &graph, size_t parallel_grain_size) {
    auto lengths = graph.edges.get_or_add<bcg_scalar_t, 1>("e_length");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) graph.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    lengths[e] = edge_length(graph, e);
                }
            }
    );
    lengths.set_dirty();
}

}