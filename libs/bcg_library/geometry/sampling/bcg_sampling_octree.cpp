//
// Created by alex on 04.01.21.
//

#include <queue>
#include "bcg_sampling_octree.h"
#include "utils/bcg_dynamic_bitset.h"

namespace bcg {

sampling_octree::sampling_octree(SamplingType sampling_type, property<VectorS<3>, 3> ref_positions,
                                 property<VectorS<3, 3>> sample_points, int leaf_size, int max_depth) : sampling_type(
        sampling_type), ref_positions(ref_positions), sample_points(sample_points), leaf_size(leaf_size), max_depth(
        max_depth) {
    build(sampling_type, positions, sample_points, leaf_size, max_depth);
}

void sampling_octree::clear() {
    storage.clear();
    indices.clear();
}

void sampling_octree::build(SamplingType sampling_type, property<VectorS<3>, 3> ref_positions,
                            property<VectorS<3, 3>> sample_points, int leaf_size, int max_depth) {
    this->sampling_type = sampling_type;
    this->ref_positions = ref_positions;
    this->sample_points = sample_points;
    this->leaf_size = leaf_size;
    this->max_depth = max_depth;
    rebuild();
}

void sampling_octree::rebuild() {
    clear();
    aabb = aligned_box3();
    for (size_t i = 0; i < ref_positions.size(); ++i) {
        aabb.grow(ref_positions[i]);
        indices[i] = i;
    }
    if (sampling_type == SamplingType::mean) {
        sampling_container.clear();
        sample_points = sampling_container.get_or_add<VectorS<3, 3>>("samples");
    }
    DynamicBitset bitset;


    struct build_node {
        build_node(aligned_box3 aabb, uint8_t depth) : aabb(std::move(aabb)), depth(depth) {}

        aligned_box3 aabb;
        uint8_t depth;
    };

    std::queue<build_node> queue;
    queue.emplace(aabb, 0);
    const auto &node = queue.front();
    storage.push_back(
            {0, positions.size() - 1, positions.size(), BCG_INVALID_ID, BCG_INVALID_ID, BCG_INVALID_ID, 0, 0});
    size_t counter = 0;
    int max_depth_ = std::min<int>(max_depth, 21);
    while (!queue.empty()) {
        auto &node = queue.front();
        if (storage[counter].size > leaf_size && node.depth < max_depth_) {
            std::vector<size_t> buckets[8];

            auto node_center = node.aabb.center();
            if (sampling_type == SamplingType::mean) {
                sampling_container.push_back();
                sample_points.back() = VectorS<3>::Zero();
                storage[counter].sampled_index = sample_points.size();
            }
            VectorS<3> center = aabb.center();
            auto poisson_radius = aabb.diagonal().norm() / 2;
            for (size_t j = storage[counter].v_start; j <= storage[counter].v_end; ++j) {
                size_t morton_code = MortonCode(node_center, positions[indices[j]]);
                buckets[morton_code].push_back(indices[j]);

                switch (sampling_type) {
                    case SamplingType::mean : {
                        sample_points.back() += ref_positions[indices[j]] / storage[counter].size;
                        break;
                    }
                    case SamplingType::closest : {
                        if (storage[counter].sampled_index == BCG_INVALID_ID && !bitset.test(indices[j])) {
                            storage[counter].sampled_index = indices[j];
                            bitset.set(indices[j]);
                        } else if ((center - ref_positions[indices[j]]).squaredNorm() <
                                   (center - ref_positions[storage[counter].sampled_index]).squaredNorm()) {
                            bitset.reset(storage[counter].sampled_index);
                            storage[counter].sampled_index = indices[j];
                        }
                        break;
                    }
                    case SamplingType::poisson : {
                        if(storage[counter].sampled_index == BCG_INVALID_ID && !bitset.test(indices[j]) && !reject(counter, aabb, ref_positions[indices[j]], poisson_radius)){
                            storage[counter].sampled_index = indices[j];
                            bitset.set(indices[j]);
                        }
                        break;
                    }
                }
            }

            switch (sampling_type) {
                case SamplingType::first : {
                    size_t child_start = storage[counter].v_start;
                    if (storage[counter].sampled_index == BCG_INVALID_ID && !bitset.test(indices[child_start])) {
                        storage[counter].sampled_index = indices[child_start];
                        bitset.set(indices[child_start]);
                    }
                    break;
                }
                case SamplingType::last : {
                    size_t child_end = storage[counter].v_end;
                    if (storage[counter].sampled_index == BCG_INVALID_ID && !bitset.test(indices[child_end])) {
                        storage[counter].sampled_index = indices[child_end];
                        bitset.set(indices[child_end]);
                    }
                    break;
                }
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
                        {child_start, child_end, child_size, BCG_INVALID_ID, BCG_INVALID_ID,
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

neighbors_query
sampling_octree::query_radius(const VectorS<3> &query_point, bcg_scalar_t radius, uint8_t search_depth) const {

}

neighbors_query sampling_octree::query_knn(const VectorS<3> &query_point, int num_closest, uint8_t search_depth) const {

}

void sampling_octree::query_radius(size_t index, const aligned_box3 &aabb, const sphere3 &sphere,
                                   neighbors_query &result_set, uint8_t search_depth) const {

}

void sampling_octree::query_knn(size_t index, const aligned_box3 &aabb, const VectorS<3> &query_point, int num_closest,
                                neighbors_query &result_set, uint8_t search_depth) const {}

bool sampling_octree::reject(size_t counter, const aligned_box3 &aabb, const VectorS<3> &point, bcg_scalar_t poisson_radius) const{
    TCSphere3AlignedBox3<Real> contains;
    if (contains(sphere, aabb)) {
        return reject_up(index, aabb, sphere);
    }

    if (index != 0) {
        auto loc_code = Base::m_internal_nodes[index].loc_code;
        if (reject_down(Base::m_internal_nodes[index].parent_index,
                        ParentBox(aabb, MortonCode3(loc_code, TreeDepth3(loc_code))),
                        sphere)) {
            return true;
        }
    }

    return reject_down(0, Base::m_root_bounds, sphere);
}

bool sampling_octree::reject_down(size_t counter, const aligned_box3 &aabb, const VectorS<3> &point, bcg_scalar_t poisson_radius) const{

}

bool sampling_octree::reject_up(size_t counter, const aligned_box3 &aabb, const VectorS<3> &point, bcg_scalar_t poisson_radius) const{

}

}