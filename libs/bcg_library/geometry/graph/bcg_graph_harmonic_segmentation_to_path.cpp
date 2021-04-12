//
// Created by alex on 12.04.21.
//

#include "bcg_graph_harmonic_segmentation_to_path.h"
#include "tbb/tbb.h"

namespace bcg{

void graph_harmonic_segmentation_to_path(halfedge_graph &graph, property<bcg_scalar_t, 1> harmonic_field,
                                         const std::vector<bcg_scalar_t> &iso_values, size_t parallel_grain_size){
    auto path = graph.edges.get_or_add<bcg_scalar_t, 1>("v_path");
    path.reset(0);

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) graph.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    auto v0 = graph.get_vertex(e, 0);
                    auto v1 = graph.get_vertex(e, 1);
                    for(const auto &iso_value : iso_values){
                        bcg_scalar_t value0 = harmonic_field[v0] - iso_value;
                        bcg_scalar_t value1 = harmonic_field[v1] - iso_value;
                        path[e] += (value0 > 0 && value1 < 0) || (value0 < 0 && value1 > 0);
                    }
                }
            }
    );
    path.set_dirty();
}

}