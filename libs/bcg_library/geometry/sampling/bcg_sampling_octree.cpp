//
// Created by alex on 04.01.21.
//

#include <queue>
#include "bcg_sampling_octree.h"
#include "utils/bcg_dynamic_bitset.h"
#include "sphere/bcg_sphere_contains_aabb.h"
#include "sphere/bcg_sphere_contains_point.h"
#include "intersect/bcg_intersect_sphere_aabb.h"
#include "aligned_box/bcg_aligned_child_hierarchy.h"
#include "utils/bcg_stl_utils.h"

namespace bcg {

std::vector<std::string> sampling_octree::type_names() {
    std::vector<std::string> names(static_cast<size_t>(SamplingType::__last__));
    names[static_cast<size_t>(SamplingType::first)] = "first";
    names[static_cast<size_t>(SamplingType::last)] = "last";
    names[static_cast<size_t>(SamplingType::mean)] = "mean";
    names[static_cast<size_t>(SamplingType::closest)] = "closest";
    names[static_cast<size_t>(SamplingType::poisson)] = "poisson";
    return names;
}

sampling_octree::sampling_octree(SamplingType sampling_type, property<VectorS<3>, 3> ref_positions, int leaf_size,
                                 int max_depth) : sampling_type(
        sampling_type), ref_positions(ref_positions), leaf_size(leaf_size), max_depth(max_depth) {
    build(sampling_type, ref_positions, leaf_size, max_depth);
}

sampling_octree::sampling_octree(SamplingType sampling_type, property<VectorS<3>, 3> ref_positions,
                                 const std::vector<size_t> &ordering, int leaf_size, int max_depth) : sampling_type(
        sampling_type), ref_positions(ref_positions), leaf_size(leaf_size), max_depth(max_depth) {
    build(sampling_type, ref_positions, ordering, leaf_size, max_depth);
}

void sampling_octree::clear() {
    storage.clear();
    current_depth = 0;
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

void sampling_octree::build(SamplingType sampling_type, property<VectorS<3>, 3> ref_positions, int leaf_size,
                            int max_depth) {
    this->sampling_type = sampling_type;
    this->ref_positions = ref_positions;
    this->leaf_size = leaf_size;
    this->max_depth = max_depth;
    this->indices.clear();
    rebuild();
}

void sampling_octree::build(SamplingType sampling_type, property<VectorS<3>, 3> ref_positions,
                            const std::vector<size_t> &ordering,
                            int leaf_size, int max_depth) {

    this->sampling_type = sampling_type;
    this->ref_positions = ref_positions;
    this->leaf_size = leaf_size;
    this->max_depth = max_depth;
    this->indices = ordering;
    rebuild();
}

void sampling_octree::rebuild() {
    clear();
    aabb = aligned_box3();
    if(indices.empty()){
        indices.resize(ref_positions.size());
        for (size_t i = 0; i < ref_positions.size(); ++i) {
            aabb.grow(ref_positions[i]);
            indices[i] = i;
        }
    }else{
        if(indices.size() != ref_positions.size()){
            std::cout << "indices.size() does not match ref_positions.size()!\n";
            return;
        }
        for (size_t i = 0; i < ref_positions.size(); ++i) {
            aabb.grow(ref_positions[i]);
        }
    }

    if (sampling_type == SamplingType::mean) {
        sampling_container.clear();
        sample_points = sampling_container.get_or_add<VectorS<3>, 3>("samples");
    }
    DynamicBitset bitset;


    struct build_node {
        build_node(aligned_box3 aabb, uint8_t depth) : aabb(std::move(aabb)), depth(depth) {}

        aligned_box3 aabb;
        uint8_t depth;
    };

    std::queue<build_node> queue;
    queue.emplace(aabb, 0);
    storage.push_back(
            {0, ref_positions.size() - 1, ref_positions.size(), BCG_INVALID_ID, BCG_INVALID_ID, BCG_INVALID_ID, 0, 0,
             0});
    size_t counter = 0;
    int max_depth_ = std::min<int>(max_depth, 21);
    while (!queue.empty()) {
        auto &node = queue.front();
        if (storage[counter].size > leaf_size && node.depth < max_depth_) {
            current_depth = node.depth > current_depth ? node.depth : current_depth;
            std::vector<size_t> buckets[8];

            auto node_center = node.aabb.center();
            if (sampling_type == SamplingType::mean) {
                storage[counter].sampled_index = sample_points.size();
                sampling_container.push_back();
                sample_points.back() = VectorS<3>::Zero();
            }
            auto poisson_radius = node.aabb.diagonal().norm() / 2;
            size_t count_sampled = 0;
            size_t child_end = storage[counter].v_end;
            for (size_t j = storage[counter].v_start; j <= storage[counter].v_end; ++j) {
                size_t point_index = indices[j];
                size_t morton_code = MortonCode(node_center, ref_positions[point_index]);
                buckets[morton_code].push_back(point_index);

                switch (sampling_type) {
                    case SamplingType::first : {
                        if (storage[counter].sampled_index == BCG_INVALID_ID && !bitset.test(point_index)) {
                            storage[counter].sampled_index = point_index;
                            bitset.set(point_index);
                        }
                        break;
                    }
                    case SamplingType::last : {
                        if (storage[counter].sampled_index == BCG_INVALID_ID && !bitset.test(indices[child_end])) {
                            storage[counter].sampled_index = indices[child_end];
                            bitset.set(indices[child_end]);
                        } else {
                            --child_end;
                        }

                        break;
                    }
                    case SamplingType::mean : {
                        sample_points.back() += ref_positions[point_index] / storage[counter].size;
                        break;
                    }
                    case SamplingType::closest : {
                        if (storage[counter].sampled_index == BCG_INVALID_ID) {
                            if (!bitset.test(point_index)) {
                                storage[counter].sampled_index = point_index;
                                bitset.set(point_index);
                            }
                        } else if ((node_center - ref_positions[point_index]).squaredNorm() <
                                   (node_center - ref_positions[storage[counter].sampled_index]).squaredNorm()) {
                            if (bitset.test(storage[counter].sampled_index)) {
                                bitset.reset(storage[counter].sampled_index);
                            }
                            storage[counter].sampled_index = point_index;
                            bitset.set(point_index);
                        }
                        break;
                    }
                    case SamplingType::poisson : {
                        auto sphere = sphere3(ref_positions[point_index], poisson_radius);
                        if (storage[counter].sampled_index == BCG_INVALID_ID && !bitset.test(point_index) &&
                            !reject(counter, node.aabb, sphere)) {
                            storage[counter].sampled_index = point_index;
                            bitset.set(point_index);
                        }
                        break;
                    }
                    case __last__:
                        break;
                }
                count_sampled += bitset.test(point_index);
            }

            if (count_sampled < storage[counter].size) {
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
                            {child_start, child_end, child_size, BCG_INVALID_ID, BCG_INVALID_ID,
                             counter, 0, static_cast<uint8_t>(node.depth + 1), octant});
                    queue.emplace(child_box(node.aabb, octant), node.depth + 1);

                    for (size_t l = 0; l < child_size; ++l) {
                        indices[child_start + l] = buckets[octant][l];
                    }

                    buckets[octant].clear();
                    child_start += child_size;

                    assert(storage.back().v_start >= storage[counter].v_start);
                    assert(storage.back().v_end <= storage[counter].v_end);
                    assert(storage.back().size <= storage[counter].size);
                }
            }
        }
        ++counter;
        queue.pop();
    }
}

neighbors_query
sampling_octree::query_radius(const VectorS<3> &query_point, bcg_scalar_t radius, uint8_t search_depth) const {
    neighbors_query result;
    query_radius(0, aabb, sphere3(query_point, radius), result, search_depth);
    return result;
}

neighbors_query sampling_octree::query_knn(const VectorS<3> &query_point, int num_closest, uint8_t search_depth) const {
    neighbors_query result;
    query_knn(0, aabb, query_point, num_closest, result, search_depth);
    return result;
}

void sampling_octree::query_radius(size_t counter, const aligned_box3 &current_aabb, const sphere3 &sphere,
                                   neighbors_query &result_set, uint8_t search_depth) const {
    assert(counter < storage.size());

    if (storage[counter].sampled_index != BCG_INVALID_ID &&
        (storage[counter].config == 0 || storage[counter].depth <= search_depth)) {
        if (sampling_type == SamplingType::mean) {
            if (contains<3>(sphere, sample_points[storage[counter].sampled_index])) {
                auto dist = (sample_points[storage[counter].sampled_index] - sphere.center).norm();
                result_set.indices.push_back(storage[counter].sampled_index);
                result_set.distances.push_back(dist);
            }
        } else {
            if (contains<3>(sphere, ref_positions[storage[counter].sampled_index])) {
                auto dist = (ref_positions[storage[counter].sampled_index] - sphere.center).norm();
                result_set.indices.push_back(storage[counter].sampled_index);
                result_set.distances.push_back(dist);
            }
        }
    }

    size_t offset = 0;
    for (uint8_t octant = 0; octant < 8; ++octant) {
        if (!child_exists(storage[counter].config, octant)) {
            continue;
        }
        auto childBox = child_box(current_aabb, octant);
        if (!intersect<3>(sphere, childBox)) {
            ++offset;
            continue;
        }
        query_radius(storage[counter].first_child_index + offset, childBox, sphere, result_set, search_depth);
        ++offset;
    }
}

void
sampling_octree::query_knn(size_t counter, const aligned_box3 &current_aabb, const VectorS<3> &query_point,
                           int num_closest,
                           neighbors_query &result_set, uint8_t search_depth) const {
    //descend to leaf
    //first order children by distance to query point

    //collect up to k points
    if (storage[counter].sampled_index != BCG_INVALID_ID &&
        (storage[counter].config == 0 || storage[counter].depth <= search_depth)) {
        if (sampling_type == SamplingType::mean) {
            auto dist = (sample_points[storage[counter].sampled_index] - query_point).norm();
            result_set.indices.push_back(storage[counter].sampled_index);
            result_set.distances.push_back(dist);
        } else {
            auto dist = (ref_positions[storage[counter].sampled_index] - query_point).norm();
            result_set.indices.push_back(storage[counter].sampled_index);
            result_set.distances.push_back(dist);
        }

        if (result_set.indices.size() >= num_closest) {
            sort_by_first(result_set.distances, result_set.indices);
            result_set.indices.resize(num_closest);
            result_set.distances.resize(num_closest);
        }
        if (storage[counter].config == 0) return;
    }

    uint8_t morton_code = MortonCode(current_aabb.center(), query_point);
    uint8_t offset = 0;
    for (uint8_t octant = 0; octant < 8; ++octant) {
        if (morton_code == octant) break;
        if (!child_exists(storage[counter].config, octant)) continue;
        ++offset;
    }

    if (child_exists(storage[counter].config, morton_code)) {
        query_knn(storage[counter].first_child_index + offset, child_box(current_aabb, morton_code), query_point,
                  num_closest, result_set, search_depth);
    }

    offset = 0;
    for (uint8_t octant = 0; octant < 8; ++octant) {
        if (!child_exists(storage[counter].config, octant)) continue;
        if (morton_code == octant) {
            ++offset;
            continue;
        }
        auto childBox = child_box(current_aabb, octant);
        if (result_set.indices.size() < num_closest ||
            intersect<3>(sphere3(query_point, result_set.distances.back()), childBox)) {
            query_knn(storage[counter].first_child_index + offset, childBox, query_point, num_closest, result_set,
                      search_depth);
        }
        ++offset;
    }
}

std::vector<VectorS<3>> sampling_octree::get_samples(uint8_t depth) const {
    std::vector<VectorS<3>> samples;
    for (const auto &node : storage) {
        if (node.depth > depth) break;
        if (node.sampled_index == BCG_INVALID_ID) continue;
        if (sampling_type == SamplingType::mean) {
            samples.push_back(sample_points[node.sampled_index]);
        } else {
            samples.push_back(ref_positions[node.sampled_index]);
        }
    }
    return samples;
}

bool sampling_octree::reject(size_t counter, const aligned_box3 &current_aabb, const sphere3 &sphere) const {
    if (contains<3>(sphere, current_aabb)) {
        return reject_up(counter, current_aabb, sphere);
    }

    if (counter != 0) {
        if (reject_down(storage[counter].parent_index, parent_box(current_aabb, storage[counter].octant),
                        sphere)) {
            return true;
        }
    }

    return reject_down(0, aabb, sphere);
}

bool sampling_octree::reject_down(size_t counter, const aligned_box3 &current_aabb, const sphere3 &sphere) const {
    if (sampling_type == SamplingType::mean) {
        if (storage[counter].sampled_index != BCG_INVALID_ID && contains<3>(sphere, sample_points[storage[counter].sampled_index])) {
            return true;
        }
    } else {
        if (storage[counter].sampled_index != BCG_INVALID_ID &&
            contains<3>(sphere, ref_positions[storage[counter].sampled_index])) {
            return true;
        }
    }


    uint8_t offset = 0;
    for (uint8_t octant = 0; octant < 8; ++octant) {
        if (!child_exists(storage[counter].config, octant)) continue;
        auto childBox = child_box(current_aabb, octant);
        size_t child_index = storage[counter].first_child_index + offset;
        if (!intersect<3>(sphere, childBox)) {
            ++offset;
            continue;
        }
        if (child_index > storage.size()) break;
        if (reject_down(child_index, childBox, sphere)) return true;
        ++offset;
    }
    return false;
}

bool sampling_octree::reject_up(size_t counter, const aligned_box3 &current_aabb, const sphere3 &sphere) const {
    if (sampling_type == SamplingType::mean && storage[counter].sampled_index != BCG_INVALID_ID && contains<3>(sphere, sample_points[storage[counter].sampled_index])) {
        return true;
    } else if (storage[counter].sampled_index != BCG_INVALID_ID && contains<3>(sphere, ref_positions[storage[counter].sampled_index])) {
        return true;
    }

    if (counter != 0) {
        return reject_up(storage[counter].parent_index, parent_box(current_aabb, storage[counter].octant), sphere);
    }
    return false;
}

}