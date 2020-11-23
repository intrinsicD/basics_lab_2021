//
// Created by alex on 23.11.20.
//

#include "bcg_mesh_vertex_barycentric_areas.h"
#include "bcg_mesh_triangle_area_from_metric.h"
#include "tbb/tbb.h"

namespace bcg{

bcg_scalar_t vertex_barycentric_area(const halfedge_mesh &mesh, vertex_handle v){
    bcg_scalar_t area(0.0);

    if (!mesh.is_isolated(v)) {
        const auto &p = mesh.positions[v];
        halfedge_handle h0, h1;
        VectorS<3> q, r, pq, pr;

        for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
            if (mesh.is_boundary(h)) {
                continue;
            }
            h0 = h;
            h1 = mesh.get_next(h0);
            q = mesh.positions[mesh.get_to_vertex(h0)];
            r = mesh.positions[mesh.get_to_vertex(h1)];
            area += triangle_area_from_metric((p-q).norm(), (q-r).norm(), (r-p).norm()) / 3.0;
        }
    }
    assert(area >= 0);
    return area;
}

void vertex_barycentric_areas(halfedge_mesh &mesh, size_t parallel_grain_size){
    auto v_barycentric_area = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_barycentric_area");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    v_barycentric_area[v] = vertex_barycentric_area(mesh, v);
                }
            }
    );
    v_barycentric_area.set_dirty();
}

}