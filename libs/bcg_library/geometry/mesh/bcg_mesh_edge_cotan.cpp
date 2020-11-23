//
// Created by alex on 23.11.20.
//

#include "bcg_mesh_edge_cotan.h"
#include "bcg_mesh_triangle_cotan_weight.h"
#include "tbb/tbb.h"

namespace bcg{

bcg_scalar_t edge_cotan(const halfedge_mesh &mesh, edge_handle e) {
    bcg_scalar_t weight = 0.0;

    const auto h0 = mesh.halfedge_graph::get_halfedge(e, 0);
    const auto h1 = mesh.halfedge_graph::get_halfedge(e, 1);

    const auto p0 = mesh.positions[mesh.get_to_vertex(h0)];
    const auto p1 = mesh.positions[mesh.get_to_vertex(h1)];

    if (!mesh.is_boundary(h0)) {
        const auto p2 = mesh.positions[mesh.get_to_vertex(mesh.get_next(h0))];
        weight += triangle_cotan(p0, p1, p2);
    }

    if (!mesh.is_boundary(h1)) {
        const auto p2 = mesh.positions[mesh.get_to_vertex(mesh.get_next(h1))];
        weight += triangle_cotan(p0, p1, p2);
    }

    assert(!std::isnan(weight));
    assert(!std::isinf(weight));

    return weight;
}

void edge_cotans(halfedge_mesh &mesh, size_t parallel_grain_size){
    auto e_cotan = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_cotan");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    e_cotan[e] = edge_cotan(mesh, e);
                }
            }
    );
    e_cotan.set_dirty();
}

}