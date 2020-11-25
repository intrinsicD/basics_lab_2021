//
// Created by alex on 25.11.20.
//

#include <queue>
#include <utility>
#include "bcg_octree.h"
#include "aligned_box/bcg_aligned_child_hierarchy.h"

namespace bcg {

inline uint8_t MortonCode(const VectorS<3> &center, const VectorS<3> &other) {
    uint8_t mortonCode = 0;
    if (other[0] > center[0]) {
        SET_BIT(mortonCode, 0);
    }
    if (other[1] > center[1]) {
        SET_BIT(mortonCode, 1);
    }
    if (other[2] > center[2]) {
        SET_BIT(mortonCode, 2);
    }
    return mortonCode;
}

inline bool ChildExists(uint8_t config, uint8_t i) {
    return CHECK_BIT(config, i);
}

inline void SetChildExists(uint8_t &config, uint8_t i) {
    SET_BIT(config, i);
}

void octree::build(property<VectorS<3>, 3> positions, int leaf_size, int max_depth) {
    successor = std::vector<vertex_handle>(positions.size());
    for (size_t i = 0; i < positions.size(); ++i) {
        aabb.grow(positions[i]);
        successor[i] = i + 1;
    }

    struct build_node {
        build_node(aligned_box3 aabb, uint8_t depth) : aabb(std::move(aabb)), depth(depth) {}

        aligned_box3 aabb;
        uint8_t depth;
    };

    std::queue<build_node> queue;
    queue.emplace(aabb, 0);
    const auto &node = queue.front();
    storage.push_back({0, positions.size() - 1, positions.size(), BCG_INVALID_ID, BCG_INVALID_ID, 0, 0});
    size_t counter = 0;
    while (!queue.empty()) {
        auto &node = queue.front();
        if (storage[counter].size > leaf_size && node.depth < max_depth) {
            std::vector<size_t> buckets[8];
            auto node_center = node.aabb.center();
            auto idx = storage[counter].v_start;
            for (size_t j = 0; j <= storage[counter].size; ++j) {
                size_t morton_code = MortonCode(node_center, positions[idx]);
                buckets[morton_code].push_back(idx);
                idx = successor[idx];
            }

            bool firsttime = true;
            for (uint8_t octant = 0; octant < 8; ++octant) {
                if (buckets[octant].empty()) continue;
                if (firsttime) {
                    firsttime = false;
                    storage[counter].first_child_index = storage.size();
                }

                SetChildExists(storage[counter].config, octant);

                storage.push_back(
                        {buckets[octant].front(), buckets[octant].back(), buckets[octant].size(), BCG_INVALID_ID,
                         counter, 0, static_cast<uint8_t>(node.depth + 1)});
                for (size_t i = 0; i < buckets[octant].size() - 1; ++i) {
                    successor[buckets[octant][i]] = buckets[octant][i + 1];
                }
                buckets[octant].clear();
                queue.emplace(child_box(aabb, octant), node.depth + 1);
            }
        }
    }
}

neighbors_query octree::query_radius(const VectorS<3> &query_point, bcg_scalar_t radius) {
    //TODO
}

neighbors_query octree::query_knn(const VectorS<3> &query_point, int num_closest) {
    //TODO
}

}