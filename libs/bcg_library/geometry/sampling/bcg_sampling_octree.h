//
// Created by alex on 04.01.21.
//

#ifndef BCG_GRAPHICS_BCG_SAMPLING_OCTREE_H
#define BCG_GRAPHICS_BCG_SAMPLING_OCTREE_H

#include <vector>
#include "point_cloud/bcg_point_cloud.h"
#include "aligned_box/bcg_aligned_box.h"
#include "sphere/bcg_sphere.h"
#include "bcg_neighbors_query.h"

namespace bcg {

struct sampling_node {
    size_t v_start, v_end;
    size_t size;
    size_t sampled_index;
    uint64_t first_child_index, parent_index;
    uint8_t config, depth, octant;
};

struct sampling_octree {
    enum SamplingType {
        first,
        last,
        mean,
        closest,
        poisson,
        __last__
    };

    static std::vector<std::string> type_names();

    sampling_octree() = default;

    sampling_octree(SamplingType sampling_type, property<VectorS<3>, 3> ref_positions, size_t leaf_size,
                    int max_depth = 20);

    sampling_octree(SamplingType sampling_type, property<VectorS<3>, 3> ref_positions,
                    const std::vector<size_t> &ordering, size_t leaf_size, int max_depth = 20);

    void clear();

    void build(SamplingType sampling_type, property<VectorS<3>, 3> ref_positions, size_t leaf_size, int max_depth = 20);

    void build(SamplingType sampling_type, property<VectorS<3>, 3> ref_positions, const std::vector<size_t> &ordering,
               size_t leaf_size, int max_depth = 20);

    void rebuild();

    neighbors_query query_radius(const VectorS<3> &query_point, bcg_scalar_t radius, uint8_t search_depth) const;

    neighbors_query query_knn(const VectorS<3> &query_point, int num_closest, uint8_t search_depth) const;

    std::vector<VectorS<3>> get_samples(uint8_t depth) const;

    property<VectorS<3>, 3> ref_positions;
    property_container sampling_container;
    property<VectorS<3>, 3> sample_points;
    aligned_box3 aabb;
    std::vector<sampling_node> storage;
    std::vector<size_t> indices;
    int max_depth, current_depth;
    size_t leaf_size;
    SamplingType sampling_type;
private:
    void query_radius(size_t counter, const aligned_box3 &aabb, const sphere3 &sphere, neighbors_query &result_set,
                      uint8_t search_depth) const;

    void query_knn(size_t counter, const aligned_box3 &aabb, const VectorS<3> &query_point, int num_closest,
                   neighbors_query &result_set, uint8_t search_depth) const;

    bool reject(size_t counter, const aligned_box3 &aabb, const sphere3 &sphere) const;

    bool reject_down(size_t counter, const aligned_box3 &aabb, const sphere3 &sphere) const;

    bool reject_up(size_t counter, const aligned_box3 &aabb, const sphere3 &sphere) const;

};

}

#endif //BCG_GRAPHICS_BCG_SAMPLING_OCTREE_H
