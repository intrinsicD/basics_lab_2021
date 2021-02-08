//
// Created by alex on 24.11.20.
//

#ifndef BCG_GRAPHICS_BCG_VOXEL_GRID_H
#define BCG_GRAPHICS_BCG_VOXEL_GRID_H

#include <unordered_map>
#include "bcg_occupancy_grid.h"

namespace bcg {

struct voxel {
    vertex_handle v_start, v_end;
    size_t size;
};

struct voxel_grid : private occupancy_grid {
    void build(property<VectorS<3>, 3> positions, const VectorI<3> &dims);

    bool is_occupied_point(const VectorS<3> &point) const;

    bool is_occupied_coord(const VectorI<3> &coord) const;

    bool is_occupied_idx(size_t idx) const;

    bool is_inside_bounds_point(const VectorS<3> &point) const;

    bool is_inside_bounds_coord(const VectorI<3> &coord) const;

    bool is_inside_bounds_idx(size_t idx) const;

    void insert_point(const VectorS<3> &point, vertex_handle v);

    void insert_coord(const VectorI<3> &coord, vertex_handle v);

    void insert_idx(size_t idx, vertex_handle v);

    size_t num_free_voxel() const;

    size_t num_occupied_voxel() const;

    size_t capacity() const;

    void clear();

    std::unordered_map<size_t, voxel> storage;
    std::vector<size_t> successor;
};

}

#endif //BCG_GRAPHICS_BCG_VOXEL_GRID_H
