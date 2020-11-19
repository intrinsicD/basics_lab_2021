//
// Created by alex on 19.11.20.
//

#include "bcg_mesh_vertex_convex_concave.h"
#include "tbb/tbb.h"

namespace bcg{

void vertex_convex_concave(halfedge_mesh &mesh, size_t parallel_grain_size){
    auto normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
    auto convex = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_convex");
    auto concave = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_concave");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    vertex_handle v = vertex_handle(i);
                    if (!mesh.is_boundary(v)){
                        VectorS<3> mean(zero3s);
                        size_t count = 0;
                        for (const auto h : mesh.halfedge_graph::get_halfedges(v)) {
                            ++count;
                            if (mesh.is_boundary(h)) continue;
                            mean += mesh.positions[mesh.get_to_vertex(h)];
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
    convex.set_dirty();
    concave.set_dirty();
}

}