//
// Created by alex on 23.11.20.
//

#include "bcg_mesh_vertex_valences.h"
#include "tbb/tbb.h"

namespace bcg{

void vertex_valences(halfedge_mesh &mesh, size_t parallel_grain_size){
    auto v_valence = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_valence");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    v_valence[v] = mesh.halfedge_graph::get_valence(v);
                }
            }
    );
    v_valence.set_dirty();
}

}