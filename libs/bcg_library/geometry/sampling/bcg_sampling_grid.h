//
// Created by alex on 08.02.21.
//

#ifndef BCG_GRAPHICS_BCG_SAMPLING_GRID_H
#define BCG_GRAPHICS_BCG_SAMPLING_GRID_H

#include <random>
#include <unordered_map>
#include "bcg_property.h"
#include "geometry/grid/bcg_voxel_grid.h"

namespace bcg {

enum class GridSamplingType {
    first,
    last,
    random,
    closest,
    medioid,
    __last__
};

inline std::vector<std::string> grid_sampling_type_names() {
    std::vector<std::string> names(static_cast<int>(GridSamplingType::__last__));
    names[static_cast<int>(GridSamplingType::first)] = "first";
    names[static_cast<int>(GridSamplingType::last)] = "last";
    names[static_cast<int>(GridSamplingType::random)] = "random";
    names[static_cast<int>(GridSamplingType::closest)] = "closest";
    names[static_cast<int>(GridSamplingType::medioid)] = "medioid";
    return names;
}

template<int D>
struct sampling_grid : public voxel_grid<D> {
    sampling_grid() = default;

    sampling_grid(const VectorI<D> &dims, const aligned_box<D> &aabb) : voxel_grid<D>(dims, aabb) {};

    ~sampling_grid() override = default;

    [[nodiscard]] virtual std::vector<size_t> get_samples_indices() const = 0;

    virtual std::vector<VectorS<D>> get_samples() const{
        std::vector<VectorS<D>> samples;
        samples.reserve(this->num_occupied_voxel());
        auto indices = get_samples_indices();
        for (const auto &idx: indices) {
            samples.push_back(this->ref_positions[idx]);
        }
        return samples;
    }

    [[nodiscard]] std::vector<VectorS<3>> get_voxel_centers() const {
        std::vector<VectorS<D>> samples;
        samples.reserve(this->num_occupied_voxel());
        for (const auto &item: this->storage) {
            samples.push_back(this->idx_to_voxel_center(item.first));
        }
        return samples;
    }
};

template<int D>
struct sample_first_grid : public sampling_grid<D> {
    sample_first_grid() = default;

    sample_first_grid(const VectorI<D> &dims, const aligned_box<D> &aabb) : sampling_grid<D>(dims, aabb) {

    }

    ~sample_first_grid() override = default;

    [[nodiscard]] std::vector<size_t> get_samples_indices() const override {
        std::vector<size_t> indices;
        indices.reserve(this->num_occupied_voxel());
        traverse(*this, [&indices](size_t, const voxel &vox) {
            indices.push_back(vox.v_start);
            return true;
        });
        return indices;
    }

    std::vector<VectorS<D>> get_samples() const override {
        std::vector<VectorS<D>> samples;
        samples.reserve(this->num_occupied_voxel());
        traverse(*this, [&samples, this](size_t, const voxel &vox) {
            samples.push_back(this->ref_positions[vox.v_start]);
            return true;
        });
        return samples;
    }
};

template<int D>
struct sample_last_grid : public sampling_grid<D> {
    sample_last_grid() = default;

    sample_last_grid(const VectorI<D> &dims, const aligned_box<D> &aabb) : sampling_grid<D>(dims, aabb) {

    }

    ~sample_last_grid() override = default;

    [[nodiscard]] std::vector<size_t> get_samples_indices() const override {
        std::vector<size_t> indices;
        indices.reserve(this->num_occupied_voxel());
        traverse(*this, [&indices](size_t, const voxel &vox) {
            indices.push_back(vox.v_end);
            return true;
        });
        return indices;
    }

    std::vector<VectorS<D>> get_samples() const override {
        std::vector<VectorS<D>> samples;
        samples.reserve(this->num_occupied_voxel());
        traverse(*this, [&samples, this](size_t, const voxel &vox) {
            samples.push_back(this->ref_positions[vox.v_end]);
            return true;
        });
        return samples;
    }
};


template<int D>
struct sample_random_grid : public sampling_grid<D> {
    sample_random_grid() = default;

    sample_random_grid(const VectorI<D> &dims, const aligned_box<D> &aabb) : sampling_grid<D>(dims, aabb) {
        srand(42);
    }

    ~sample_random_grid() override = default;

    void resample(size_t seed) {
        srand(seed);
    }

    [[nodiscard]] std::vector<size_t> get_samples_indices() const override {
        std::vector<size_t> indices;
        indices.reserve(this->num_occupied_voxel());
        traverse(*this, [&indices, this](size_t, const voxel &item) {
            size_t rand_steps = rand() % item.size;
            vertex_handle current_idx = item.v_start;
            for (size_t i = 0; i < rand_steps; ++i) {
                current_idx = this->successor[current_idx];
            }
            indices.emplace_back(current_idx);
            return true;
        });
        return indices;
    }
};

template<int D>
struct sample_closest_grid : public sampling_grid<D> {
    sample_closest_grid() = default;

    sample_closest_grid(const VectorI<D> &dims, const aligned_box<D> &aabb) : sampling_grid<D>(dims, aabb) {

    }

    ~sample_closest_grid() override = default;

    [[nodiscard]] std::vector<size_t> get_samples_indices() const override {
        std::vector<size_t> indices;
        indices.reserve(this->num_occupied_voxel());
        traverse(*this, [&indices, this](size_t idx, const voxel &item) {
            VectorS<D> center = this->idx_to_voxel_center(idx);
            bcg_scalar_t current_min_dist = scalar_max;
            size_t current_min_idx = item.v_start;
            vertex_handle current_idx = item.v_start;
            for (size_t i = 0; i < item.size; ++i) {
                bcg_scalar_t dist = (this->ref_positions[current_idx] - center).squaredNorm();
                if (dist < current_min_dist) {
                    current_min_idx = current_idx;
                }
                current_idx = this->successor[current_idx];
            }

            indices.emplace_back(current_min_idx);
            return true;
        });
        return indices;
    }
};

template<int D>
struct sample_medioid_grid : public sampling_grid<D> {
    sample_medioid_grid() = default;

    sample_medioid_grid(const VectorI<D> &dims, const aligned_box<D> &aabb) : sampling_grid<D>(dims, aabb) {

    }

    ~sample_medioid_grid() override = default;

    //returns medioid_indices
    [[nodiscard]] std::vector<size_t> get_samples_indices() const override {
        std::vector<size_t> indices;
        indices.reserve(this->num_occupied_voxel());
        traverse(*this, [&indices, this](size_t, const voxel &item) {
            VectorS<D> mean = VectorS<D>::Zero();
            vertex_handle current_idx = item.v_start;
            for (size_t i = 0; i < item.size; ++i) {
                mean += this->ref_positions[current_idx] / item.size;
                current_idx = this->successor[current_idx];
            }
            bcg_scalar_t current_min_dist = scalar_max;
            size_t current_min_idx = item.v_start;
            current_idx = item.v_start;
            for (size_t i = 0; i < item.size; ++i) {
                bcg_scalar_t dist = (this->ref_positions[current_idx] - mean).squaredNorm();
                if (dist < current_min_dist) {
                    current_min_idx = current_idx;
                }
                current_idx = this->successor[current_idx];
            }
            indices.emplace_back(current_min_idx);
            return true;
        });
        return indices;
    }
};

template<int D>
void traverse(sampling_grid<D> &grid, std::function<bool(size_t, voxel &)> &function) {
    for (auto &item : grid.storage) {
        function(item.first, item.second);
    }
}

template<int D>
void traverse(const sampling_grid<D> &grid, const std::function<bool(size_t, const voxel &)> &function) {
    for (const auto &item : grid.storage) {
        function(item.first, item.second);
    }
}

using sample_first_grid2 = sample_first_grid<2>;
using sample_last_grid2 = sample_last_grid<2>;
using sample_random_grid2 = sample_random_grid<2>;
using sample_closest_grid2 = sample_closest_grid<2>;
using sample_mean_grid2 = sample_medioid_grid<2>;

using sample_first_grid3 = sample_first_grid<3>;
using sample_last_grid3 = sample_last_grid<3>;
using sample_random_grid3 = sample_random_grid<3>;
using sample_closest_grid3 = sample_closest_grid<3>;
using sample_medioid_grid3 = sample_medioid_grid<3>;

}

#endif //BCG_GRAPHICS_BCG_SAMPLING_GRID_H
