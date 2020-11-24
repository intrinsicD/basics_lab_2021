//
// Created by alex on 24.11.20.
//

#ifndef BCG_GRAPHICS_BCG_BASE_GRID_H
#define BCG_GRAPHICS_BCG_BASE_GRID_H

#include "geometry/aligned_box/bcg_aligned_box.h"
#include "geometry/bcg_property.h"

namespace bcg {

struct base_grid {
    size_t to_idx(const VectorS<3> &item) const;

    VectorI<3> to_coord(const VectorS<3> &item) const;

    VectorS<3> to_voxel_center(const VectorS<3> &item) const;

    VectorS<3> to_voxel_min(const VectorS<3> &item) const;

    VectorS<3> to_voxel_max(const VectorS<3> &item) const;

    size_t coord_to_idx(const VectorI<3> &coord) const;

    VectorS<3> coord_to_voxel_center(const VectorI<3> &coord) const;

    VectorS<3> coord_to_voxel_min(const VectorI<3> &coord) const;

    VectorS<3> coord_to_voxel_max(const VectorI<3> &coord) const;

    VectorI<3> idx_to_coord(size_t idx) const;

    VectorS<3> idx_to_voxel_center(size_t i) const;

    VectorS<3> idx_to_voxel_min(size_t i) const;

    VectorS<3> idx_to_voxel_max(size_t i) const;

    VectorS<3> voxel_side_length() const;

    size_t capacity() const;

    bool is_boundary(const VectorI<3> &coord) const;

    std::vector<VectorI<3>> boundary_coords() const;

    aligned_box3 voxel_bounds(const VectorI<3> &coord) const;

    void build_aabb(property<VectorS<3>, 3> positions);

    VectorI<3> dims;
    aligned_box3 aabb;
};

}

#endif //BCG_GRAPHICS_BCG_BASE_GRID_H
