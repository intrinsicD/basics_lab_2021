//
// Created by alex on 25.11.20.
//

#include <queue>
#include <utility>
#include "bcg_octree.h"
#include "aligned_box/bcg_aligned_child_hierarchy.h"
#include "sphere/bcg_sphere.h"
#include "sphere/bcg_sphere_contains_aabb.h"
#include "intersect/bcg_intersect_sphere_aabb.h"
#include "utils/bcg_stl_utils.h"

namespace bcg {

octree::octree(property<VectorS<3>, 3> positions, size_t leaf_size, int max_depth){
    build(positions, leaf_size, max_depth);
}

void octree::clear(){
    aabb = aligned_box3();
    storage.clear();
}

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

inline bool child_exists(uint8_t config, uint8_t i) {
    return CHECK_BIT(config, i);
}

inline void SetChildExists(uint8_t &config, uint8_t i) {
    SET_BIT(config, i);
}

void octree::build(property<VectorS<3>, 3> positions, size_t leaf_size, int max_depth) {
    clear();
    this->max_depth = max_depth;
    this->leaf_size = leaf_size;
    this->positions = positions;
    indices = std::vector<size_t>(positions.size());
    for (size_t i = 0; i < positions.size(); ++i) {
        aabb.grow(positions[i]);
        indices[i] = i;
    }

    struct build_node {
        build_node(aligned_box3 aabb, uint8_t depth) : aabb(std::move(aabb)), depth(depth) {}

        aligned_box3 aabb;
        uint8_t depth;
    };

    std::queue<build_node> queue;
    queue.emplace(aabb, 0);
    storage.push_back({0, positions.size() - 1, positions.size(), BCG_INVALID_ID, BCG_INVALID_ID, 0, 0});
    size_t counter = 0;
    int max_depth_ = std::min<int>(max_depth, 21);
    while (!queue.empty()) {
        auto &node = queue.front();
        if (storage[counter].size > leaf_size && node.depth < max_depth_) {
            std::vector<size_t> buckets[8];

            auto node_center = node.aabb.center();
            for (size_t j = storage[counter].v_start; j <= storage[counter].v_end; ++j) {
                size_t morton_code = MortonCode(node_center, positions[indices[j]]);
                buckets[morton_code].push_back(indices[j]);
            }

            bool firsttime = true;
            size_t child_start = storage[counter].v_start;

            for (uint8_t octant = 0; octant < 8; ++octant) {
                if (buckets[octant].empty()) continue;
                if (firsttime) {
                    firsttime = false;
                    storage[counter].first_child_index = storage.size();
                }

                SetChildExists(storage[counter].config, octant);

                size_t child_size = buckets[octant].size();
                size_t child_end = child_start + child_size - 1;

                storage.push_back(
                        {child_start, child_end, child_size, BCG_INVALID_ID,
                         counter, 0, static_cast<uint8_t>(node.depth + 1)});
                for (size_t l = 0; l < child_size; ++l) {
                    indices[child_start + l] = buckets[octant][l];
                }

                buckets[octant].clear();
                child_start += child_size;
                queue.emplace(child_box(node.aabb, octant), node.depth + 1);
                assert(storage.back().v_start >= storage[counter].v_start);
                assert(storage.back().v_end <= storage[counter].v_end);
                assert(storage.back().size <= storage[counter].size);
            }
        }
        ++counter;
        queue.pop();
    }
}

//test if sphere overlaps with aabb
/*bool overlaps(const sphere3 &sphere, const aligned_box3 &aabb) {
    VectorS<3> xyz = (sphere.center - aabb.center()).cwiseAbs() - aabb.halfextent();
    if (xyz[0] > sphere.radius || xyz[1] > sphere.radius || xyz[2] > sphere.radius) return false;
    int num_less_extent = (xyz[0] < 0) + (xyz[1] < 0) + (xyz[2] < 0);
    if (num_less_extent > 1) return true;
    xyz.cwiseMax(0);
    return xyz.squaredNorm() < sphere.radius * sphere.radius;
}*/

neighbors_query octree::query_radius(const VectorS<3> &query_point, bcg_scalar_t radius) const {
    neighbors_query result_set;
    query_radius(0, aabb, sphere3(query_point, radius), result_set);
    return result_set;
}

neighbors_query octree::query_knn(const VectorS<3> &query_point, size_t num_closest) const {
    neighbors_query result_set;
    query_knn(0, aabb, query_point, num_closest, result_set);
    return result_set;
}

void
octree::query_radius(size_t index, const aligned_box3 &aabb, const sphere3 &sphere, neighbors_query &result_set) const {
    if (index >= storage.size()) return;
    if (contains<3>(sphere, aabb)) {
        for (size_t j = storage[index].v_start; j <= storage[index].v_end; ++j) {
            result_set.indices.push_back(indices[j]);
        }
        return;
    }

    if (storage[index].depth == max_depth || storage[index].config == 0) {
        bcg_scalar_t sqr_radius = sphere.radius * sphere.radius;
        for (size_t j = storage[index].v_start; j <= storage[index].v_end; ++j) {
            bcg_scalar_t sqr_distance = (positions[indices[j]] - sphere.center).squaredNorm();
            if (sqr_distance <= sqr_radius) {
                result_set.indices.push_back(indices[j]);
            }
        }
        return;
    }

    uint8_t offset = 0;
    for (uint8_t octant = 0; octant < 8; ++octant) {
        if (!child_exists(storage[index].config, octant)) {
            continue;
        }
        aligned_box3 cbox = child_box(aabb, octant);
        if (intersect<3>(sphere, cbox)) {
            query_radius(storage[index].first_child_index + offset, cbox, sphere, result_set);
        }
        ++offset;
    }
}

void octree::query_knn(size_t index, const aligned_box3 &aabb, const VectorS<3> &query_point, size_t num_closest,
                       neighbors_query &result_set) const {
    if (storage[index].depth == max_depth || storage[index].config == 0) {
        for (size_t j = storage[index].v_start; j <= storage[index].v_end; ++j) {
            bcg_scalar_t sqr_distance = (positions[indices[j]] - query_point).squaredNorm();
            result_set.indices.push_back(indices[j]);
            result_set.distances.push_back(sqr_distance);
        }
        if (result_set.indices.size() >= num_closest) {
            sort_by_first(result_set.distances, result_set.indices);
            result_set.distances.resize(num_closest);
            result_set.indices.resize(num_closest);
        }
        return;
    }

    uint8_t morton_code = MortonCode(aabb.center(), query_point);
    uint8_t offset = 0;
    for (uint8_t i = 0; i < 8; ++i) {
        if (morton_code == i) break;
        if (!child_exists(storage[index].config, i)) continue;
        ++offset;
    }
    if (child_exists(storage[index].config, morton_code)) {
        query_knn(storage[index].first_child_index + offset, child_box(aabb, morton_code), query_point, num_closest,
                  result_set);
    }

    for (uint8_t octant = 0; octant < 8; ++octant) {
        if (!child_exists(storage[index].config, octant)) {
            continue;
        }
        if (morton_code == octant) {
            ++offset;
            continue;
        }
        aligned_box3 cbox = child_box(aabb, octant);

        if (result_set.indices.size() < num_closest ||
            intersect<3>(sphere3(query_point, result_set.distances.back()), cbox)) {
            query_knn(storage[index].first_child_index + offset, cbox, query_point, num_closest, result_set);
        }
        ++offset;
    }
}

}