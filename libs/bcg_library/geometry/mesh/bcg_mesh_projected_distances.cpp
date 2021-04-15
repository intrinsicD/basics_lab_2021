//
// Created by alex on 15.04.21.
//

#include "bcg_mesh_projected_distances.h"
#include "kdtree/bcg_triangle_kdtree.h"
#include "tbb/tbb.h"

namespace bcg{

void mesh_projected_distances(halfedge_mesh &source, halfedge_mesh &target, size_t parallel_grain_size) {
    triangle_kdtree kd_tree = triangle_kdtree(target, 10);
    auto v_projected_distances = source.vertices.get_or_add<bcg_scalar_t, 1>("v_projected_distances");
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) source.vertices.size(), parallel_grain_size),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    auto v = vertex_handle(i);
                    triangle_kdtree::NearestNeighbor nn = kd_tree.nearest(source.positions[v]);
                    v_projected_distances[v] = (nn.result.closest - source.positions[v]).norm();
                }
            }
    );
    v_projected_distances.set_dirty();
}

}