//
// Created by alex on 19.11.20.
//

#include "bcg_mesh_vertex_convex_concave.h"
#include "bcg_mesh_edge_cotan.h"
#include "tbb/tbb.h"

namespace bcg{

void post_smoothing_convex_concave(halfedge_mesh &mesh, int post_smoothing_steps, size_t parallel_grain_size) {
    // properties
    auto v_feature = mesh.vertices.get_or_add<bool, 1>("v_feature");

    auto v_convex = mesh.vertices.get<bcg_scalar_t, 1>("v_convex");
    auto v_concave = mesh.vertices.get<bcg_scalar_t, 1>("v_concave");

    // precompute cotan weight per edge
    edge_cotans(mesh, parallel_grain_size);

    auto e_cotan = mesh.edges.get_or_add<bcg_scalar_t, 1>("e_cotan");

    for (int i = 0; i < post_smoothing_steps; ++i) {
        tbb::parallel_for(
                tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
                [&](const tbb::blocked_range<uint32_t> &range) {
                    for (uint32_t i = range.begin(); i != range.end(); ++i) {
                        auto v = vertex_handle(i);
                        if (v_feature && v_feature[v]) {

                        } else {
                            bcg_scalar_t convex = 0.0;
                            bcg_scalar_t concave = 0.0;
                            bcg_scalar_t sum_weights = 0.0;

                            for (const auto vh : mesh.halfedge_graph::get_halfedges(v)) {
                                auto tv = mesh.get_to_vertex(vh);

                                // don't consider feature vertices (high curvature)
                                if (v_feature && v_feature[tv]) {
                                    continue;
                                }

                                bcg_scalar_t weight = fmax(0.0f, e_cotan[mesh.get_edge(vh)]);
                                sum_weights += weight;
                                convex += weight * v_convex[tv];
                                concave += weight * v_concave[tv];
                            }

                            if (sum_weights) {
                                v_convex[v] = convex / sum_weights;
                                v_concave[v] = concave / sum_weights;
                            }
                        }

                    }
                }
        );
    }

    v_convex.set_dirty();
    v_concave.set_dirty();
}

void vertex_convex_concave(halfedge_mesh &mesh,int post_smoothing_steps, bool two_ring_neighborhood, size_t parallel_grain_size){
    auto normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
    auto convex = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_convex");
    auto concave = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_concave");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    vertex_handle v = vertex_handle(i);
                    if (!mesh.is_boundary(v) && !mesh.is_isolated(v)){
                        VectorS<3> mean(zero3s);
                        size_t count = 0;

                        std::vector<vertex_handle> neighborhood;
                        neighborhood.reserve(15);

                        neighborhood.clear();
                        neighborhood.push_back(v);
                        if (two_ring_neighborhood) {
                            for (const auto vv : mesh.halfedge_graph::get_vertices(v)) {
                                neighborhood.push_back(vv);
                            }
                        }
                        for (const auto &nit : neighborhood) {
                            for (const auto h : mesh.halfedge_graph::get_halfedges(nit)) {
                                if (mesh.is_boundary(h)) continue;
                                ++count;
                                mean += mesh.positions[mesh.get_to_vertex(h)];
                            }
                        }
                        if(count != 0){
                            mean /= count;
                        }

                        convex[v] = (mesh.positions[v] - mean).dot(normals[v]);
                        concave[v] =  -convex[v];
                    }else{
                        convex[v] = 0;
                        concave[v] =  0;
                    }

                }
            }
    );

    post_smoothing_convex_concave(mesh, post_smoothing_steps, parallel_grain_size);

    convex.set_dirty();
    concave.set_dirty();
}

}