//
// Created by alex on 24.11.20.
//

#ifndef BCG_GRAPHICS_BCG_OCCUPANCY_GRID_H
#define BCG_GRAPHICS_BCG_OCCUPANCY_GRID_H

#include "bcg_base_grid.h"
#include "utils/bcg_dynamic_bitset.h"
#include "bcg_neighbors_query.h"

namespace bcg {

struct occupancy_grid : public base_grid {
    occupancy_grid() = default;

    occupancy_grid(const VectorI<3> &dims, const aligned_box3 &aabb);

    void build(property<VectorS<3>, 3> positions, const VectorI<3> &dims);

    bool is_occupied_point(const VectorS<3> &point) const;

    bool is_occupied_coord(const VectorI<3> &coord) const;

    bool is_occupied_idx(size_t idx) const;

    bool is_inside_bounds_point(const VectorS<3> &point) const;

    bool is_inside_bounds_coord(const VectorI<3> &coord) const;

    bool is_inside_bounds_idx(size_t idx) const;

    void mark_occupied_point(const VectorS<3> &point);

    void mark_occupied_coord(const VectorI<3> &coord);

    void mark_occupied_idx(size_t idx);

    void mark_free_point(const VectorS<3> &point);

    void mark_free_coord(const VectorI<3> &coord);

    void mark_free_idx(size_t idx);

    size_t num_free_voxel() const;

    size_t num_occupied_voxel() const;

    size_t capacity() const;

    virtual void clear();

    DynamicBitset occupied;
};

void traverse(occupancy_grid &grid, const std::function<bool(size_t, occupancy_grid &)> &function);

void traverse_parallel(occupancy_grid &grid, const std::function<bool(size_t, occupancy_grid &)> &function);

neighbors_query query_kring(const occupancy_grid &grid, const VectorS<3> &query_point, size_t k);

neighbors_query query_knn(const occupancy_grid &grid, const VectorS<3> &query_point, size_t num_closest);

neighbors_query query_radius(const occupancy_grid &grid, const VectorS<3> &query_point, bcg_scalar_t radius);

std::vector<VectorS<3>> get_vertices(const occupancy_grid &grid);

std::vector<VectorI<2>> get_edges(const occupancy_grid &grid);

}

#endif //BCG_GRAPHICS_BCG_OCCUPANCY_GRID_H
