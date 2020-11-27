//
// Created by alex on 24.11.20.
//

#ifndef BCG_GRAPHICS_BCG_OCTREE_H
#define BCG_GRAPHICS_BCG_OCTREE_H

#include "math/vector/bcg_vector.h"
#include "bcg_property.h"
#include "aligned_box/bcg_aligned_box.h"
#include "sphere/bcg_sphere.h"
#include "bcg_neighbors_query.h"

namespace bcg{

struct octree_node{
    vertex_handle v_start, v_end;
    size_t size;
    uint64_t first_child_index, parent_index;
    uint8_t config, depth;
};

struct octree{
    void build(property<VectorS<3>, 3> positions, int leaf_size, int max_depth = 21);

    neighbors_query query_radius(const VectorS<3> &query_point, bcg_scalar_t radius);

    neighbors_query query_knn(const VectorS<3> &query_point, int num_closest);

    property<VectorS<3>, 3> positions;
    aligned_box3 aabb;
    std::vector<octree_node> storage;
    std::vector<vertex_handle> indices;
    int max_depth;
private:
    void query_radius(size_t index, const aligned_box3 &aabb, const sphere3 &sphere, neighbors_query &result_set);

    void query_knn(size_t index, const aligned_box3 &aabb, const VectorS<3> &query_point, int num_closest, neighbors_query &result_set);
};

}

#endif //BCG_GRAPHICS_BCG_OCTREE_H
