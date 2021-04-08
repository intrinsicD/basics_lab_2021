//
// Created by alex on 23.11.20.
//


#include "bcg_mesh_vertex_cotan.h"
#include "bcg_mesh_edge_cotan.h"
#include "tbb/tbb.h"

namespace bcg{

bcg_scalar_t vertex_cotan(halfedge_mesh &mesh, vertex_handle v){
    bcg_scalar_t weight = 0;
    for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
        weight += edge_cotan(mesh, mesh.get_edge(h));
    }
    return weight;
}

void vertex_cotans(halfedge_mesh &mesh, size_t parallel_grain_size){
    auto v_cotan = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_laplacian_cotan");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    v_cotan[v] = vertex_cotan(mesh, v);
                }
            }
    );
    v_cotan.set_dirty();
}

}