//
// Created by alex on 23.11.20.
//

#include "bcg_mesh_edge_fujiwara.h"
#include "tbb/tbb.h"

namespace bcg{

bcg_scalar_t edge_fujiwara(const halfedge_mesh &mesh, edge_handle e){
    assert(mesh.get_length(e) != 0);
    return 1.0 / std::max<bcg_scalar_t>(mesh.get_length(e), scalar_eps);
}

void edge_fujiwaras(halfedge_mesh &mesh, size_t parallel_grain_size){
    auto e_fujiwara = mesh.edges.get_or_add<bcg_scalar_t , 1>("e_fujiwara");

    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) mesh.edges.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto e = edge_handle(i);
                    e_fujiwara[e] = edge_fujiwara(mesh, e);
                }
            }
    );
    e_fujiwara.set_dirty();
}

}
