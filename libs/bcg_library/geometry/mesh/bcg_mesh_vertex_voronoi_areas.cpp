//
// Created by alex on 23.11.20.
//

#include "bcg_mesh_vertex_voronoi_areas.h"
#include "bcg_mesh_triangle_area_from_metric.h"
#include "bcg_mesh_triangle_cotan_weight.h"
#include "tbb/tbb.h"

namespace bcg{

bcg_scalar_t vertex_voronoi_area(const halfedge_mesh &mesh, vertex_handle v) {
    bcg_scalar_t area(0.0);

    if (!mesh.is_isolated(v)) {
        halfedge_handle h0, h1, h2;
        VectorS<3> p, q, r, pq, qr, pr;
        bcg_scalar_t dotp, dotq, dotr, triArea;
        bcg_scalar_t cotq, cotr;

        for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
            h0 = h;
            h1 = mesh.get_next(h0);
            h2 = mesh.get_next(h1);

            if (mesh.is_boundary(h0))
                continue;

            // three vertex positions
            p = mesh.positions[mesh.get_to_vertex(h2)];
            q = mesh.positions[mesh.get_to_vertex(h0)];
            r = mesh.positions[mesh.get_to_vertex(h1)];

            // edge vectors
            pq = q - p;
            qr = r - q;
            pr = r - p;

            // compute and check triangle area
            triArea = triangle_area_from_metric(pq.norm(), qr.norm(), pr.norm());
            if (triArea <= scalar_eps){
                continue;
            }
            assert(triArea > 0);
            // dot products for each corner (of its two emanating edge vectors)
            dotp = pq.dot(pr);
            dotq = -qr.dot(pq);
            dotr = qr.dot(pr);

            // angle at p is obtuse
            if (dotp < 0.0) {
                area += 0.25 * triArea;
            }

                // angle at q or r obtuse
            else if (dotq < 0.0 || dotr < 0.0) {
                area += 0.125 * triArea;
            }

                // no obtuse angles
            else {
                // cot(angle) = cos(angle)/sin(angle) = dot(A,B)/norm(cross(A,B))
                cotq = dotq / triArea;
                cotr = dotr / triArea;

                // clamp cot(angle) by clamping angle to [1,179]
                area += 0.125 * (pr.squaredNorm() * clamp_cot(cotq) + pq.squaredNorm() * clamp_cot(cotr));
            }
        }
    }

    assert(!std::isnan(area));
    assert(!std::isinf(area));
    assert(area >= 0);
    return area;
}

void vertex_voronoi_areas(halfedge_mesh &mesh, size_t parallel_grain_size){
    auto v_voronoi_area = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_voronoi_area");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    v_voronoi_area[v] = vertex_voronoi_area(mesh, v);
                }
            }
    );
    v_voronoi_area.set_dirty();
}

}