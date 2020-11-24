//
// Created by alex on 24.11.20.
//

#ifndef BCG_GRAPHICS_BCG_OCCUPANCY_GRID_H
#define BCG_GRAPHICS_BCG_OCCUPANCY_GRID_H

#include "bcg_base_grid.h"
#include "utils/bcg_dynamic_bitset.h"

namespace bcg{

struct occupancy_grid : public base_grid{
    void build(property<VectorS<3>, 3> positions, const VectorI<3> &dims);

    bool is_occupied(const VectorS<3> &point) const;
    bool is_occupied(const VectorI<3> &coord) const;
    bool is_occupied(size_t idx) const;
    bool is_inside_bounds(const VectorS<3> &point) const;
    bool is_inside_bounds(const VectorI<3> &coord) const;
    bool is_inside_bounds(size_t idx) const;
    void mark_occupied(const VectorS<3> &point);
    void mark_occupied(const VectorI<3> &coord);
    void mark_occupied(size_t idx);
    void mark_free(const VectorS<3> &point);
    void mark_free(const VectorI<3> &coord);
    void mark_free(size_t idx);
    size_t num_free_voxel() const;
    size_t num_occupied_voxel() const;
    size_t capacity() const;
    void clear();

    DynamicBitset occupied;
};

}

#endif //BCG_GRAPHICS_BCG_OCCUPANCY_GRID_H
