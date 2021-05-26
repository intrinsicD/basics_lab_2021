//
// Created by alex on 24.11.20.
//

#ifndef BCG_GRAPHICS_BCG_VOXEL_GRID_H
#define BCG_GRAPHICS_BCG_VOXEL_GRID_H

#include <unordered_map>
#include "bcg_occupancy_grid.h"
#include "bcg_property.h"
#include "bcg_property_map_eigen.h"

namespace bcg {

struct voxel {
    vertex_handle v_start, v_end;
    size_t size;
};

template<int D>
struct voxel_grid : public occupancy_grid<D> {
    voxel_grid() = default;

    voxel_grid(const VectorI<D> &dims, const aligned_box<D> &aabb) : occupancy_grid<D>(dims, aabb) {

    }

    ~voxel_grid() override = default;

    void build_successors(size_t num_points) {
        successor.resize(num_points);
        for (size_t i = 0; i < num_points; ++i) {
            successor[i] = i + 1;
        }
        successor.back() = 0;
    }

    void build(property<VectorS<D>, D> positions) override {
        ref_positions = vertices.template add(positions);
        build_successors(ref_positions.size());
        for (size_t i = 0; i < ref_positions.size(); ++i) {
            insert_point(ref_positions[i], vertex_handle(i));
        }
    }

    void build(const std::vector<VectorS<D>> &positions) override {
        ref_positions = vertices.template get_or_add<VectorS<D>, D>("ref_positions");
        ref_positions.vector() = positions;
        build_successors(ref_positions.size());
        for (size_t i = 0; i < ref_positions.size(); ++i) {
            insert_point(ref_positions[i], vertex_handle(i));
        }
    }

    void build(const MatrixS<-1, D> &positions) override {
        ref_positions = vertices.template get_or_add<VectorS<D>, D>("ref_positions");
        Map(ref_positions) = positions;
        build_successors(ref_positions.size());
        for (size_t i = 0; i < ref_positions.size(); ++i) {
            insert_point(ref_positions[i], vertex_handle(i));
        }
    }

    void insert_point(const VectorS<D> &point, vertex_handle v) {
        insert_coord(this->to_coord(point), v);
    }

    void insert_coord(const VectorI<D> &coord, vertex_handle v) {
        insert_idx(this->coord_to_idx(coord), v);
    }

    void insert_idx(size_t idx, vertex_handle v) {
        if (!this->is_inside_bounds_idx(idx)) return;
        if (!this->is_occupied_idx(idx)) {
            storage[idx] = {v, v, 1};
            this->mark_occupied_idx(idx);
        } else {
            auto &item = storage[idx];
            while (successor.size() <= item.v_end) {
                successor.push_back(successor.size() + 1);
            }
            successor[item.v_end] = v;
            item.v_end = v;
            ++item.size;
        }
    }

    void clear() override {
        occupancy_grid<D>::clear();
        successor.clear();
        storage.clear();
        vertices.template remove(ref_positions);
        ref_positions = property<VectorS<D>, D>();
    }

    std::vector<vertex_handle> indices_in_voxel(size_t idx) const {
        if (!this->is_occupied_idx(idx)) return {};
        const auto &item = storage.at(idx);
        std::vector<vertex_handle> indices(item.size);
        vertex_handle current_idx = item.v_start;
        for (size_t i = 0; i < item.size; ++i) {
            indices.push_back(current_idx);
            current_idx = successor[current_idx];
        }
        return indices;
    }

    std::vector<vertex_handle> indices_in_voxel(const VectorI<D> &coord) const {
        return indices_in_voxel(this->coord_to_idx(coord));
    }

    MatrixS<-1, D> points_in_voxel(size_t idx) const {
        if (!this->is_occupied_idx(idx)) return MatrixS<-1, D>();
        const auto &item = storage.at(idx);
        MatrixS<-1, D> points(item.size, this->dims.size());
        vertex_handle current_idx = item.v_start;
        for (size_t i = 0; i < item.size; ++i) {
            points.row(i) = ref_positions[current_idx];
            current_idx = successor[current_idx];
        }
        return points;
    }

    MatrixS<-1, D> points_in_voxel(const VectorI<D> &coord) const {
        return points_in_voxel(this->coord_to_idx(coord));
    }

    property<VectorS<D>, D> ref_positions;
    vertex_container vertices;
    std::unordered_map<size_t, voxel> storage;
    std::vector<size_t> successor;
};

template<int D>
void traverse(voxel_grid<D> &grid, const std::function<bool(size_t, voxel &)> &function) {
    for (auto &item : grid.storage) {
        function(item.first, item.second);
    }
}

template<int D>
void traverse(const voxel_grid<D> &grid, const std::function<bool(size_t, const voxel &)> &function) {
    for (const auto &item : grid.storage) {
        function(item.first, item.second);
    }
}

using voxel_grid2 = voxel_grid<2>;
using voxel_grid3 = voxel_grid<3>;

}

#endif //BCG_GRAPHICS_BCG_VOXEL_GRID_H
