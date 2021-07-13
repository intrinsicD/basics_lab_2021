//
// Created by alex on 13.07.21.
//

#include "bcg_mesh_vertex_divergence.h"
#include "bcg_mesh_triangle_cotan_weight.h"
#include "tbb/tbb.h"

namespace bcg {

bcg_scalar_t vertex_divergence(halfedge_mesh &mesh, vertex_handle v, property<VectorS<3>, 3> fgradient) {
    bcg_scalar_t divergence_value = 0;
    for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
        auto vth = mesh.get_to_vertex(h);
        auto hn = mesh.get_next(h);
        auto vthn = mesh.get_to_vertex(hn);
        auto f = mesh.get_face(h);
        auto cot_theta1 = triangle_cotan(mesh.positions[v], mesh.positions[vthn], mesh.positions[vth]);
        auto cot_theta2 = triangle_cotan(mesh.positions[v], mesh.positions[vth], mesh.positions[vthn]);
        divergence_value += cot_theta1 * (mesh.positions[vthn] - mesh.positions[v]).dot(fgradient[f]) +
                            cot_theta2 * (mesh.positions[vth] - mesh.positions[v]).dot(fgradient[f]);
    }
    return 0.5 * divergence_value;
}

property<bcg_scalar_t, 1>
vertex_divergences(halfedge_mesh &mesh, property<VectorS<3>, 3> fgradient, size_t parallel_grain_size) {
    auto v_divergences = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_divergence");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    v_divergences[v] = vertex_divergence(mesh, v, fgradient);
                }
            }
    );
    v_divergences.set_dirty();
    return v_divergences;
}

}