//
// Created by alex on 24.11.20.
//

#include "bcg_base_grid.h"

namespace bcg{

size_t base_grid::to_idx(const VectorS<3> &item) const { return coord_to_idx(to_coord(item)); }

VectorI<3> base_grid::to_coord(const VectorS<3> &item) const {
    return (dims.template cast<bcg_scalar_t>().array() *
            ((item - aabb.min).array() / aabb.diagonal().array())).template cast<bcg_index_t>().cwiseMin(
            dims.array() - 1).cwiseMax(0);
}

VectorS<3> base_grid::to_voxel_center(const VectorS<3> &item) const {
    return coord_to_voxel_center(to_coord(item));
}

VectorS<3> base_grid::to_voxel_min(const VectorS<3> &item) const {
    return coord_to_voxel_min(to_coord(item));
}

VectorS<3> base_grid::to_voxel_max(const VectorS<3> &item) const {
    return coord_to_voxel_max(to_coord(item));
}

size_t base_grid::coord_to_idx(const VectorI<3> &coord) const {
    size_t result = coord[0];
    size_t prod = dims[0];
    for (size_t i = 1; i < 3; ++i) {
        result += coord[i] * prod;
        prod *= dims[i];
    }
    return result;
}

VectorS<3> base_grid::coord_to_voxel_center(const VectorI<3> &coord) const {
    return coord_to_voxel_min(coord) + voxel_side_length() / 2;
}

VectorS<3> base_grid::coord_to_voxel_min(const VectorI<3> &coord) const {
    return aabb.min.array() + coord.template cast<bcg_scalar_t>().array() * voxel_side_length().array();
}

VectorS<3> base_grid::coord_to_voxel_max(const VectorI<3> &coord) const {
    return coord_to_voxel_min(coord) + voxel_side_length();
}

VectorI<3> base_grid::idx_to_coord(size_t idx) const {
    VectorI<3> coord;
    coord[0] = idx % dims[0];
    size_t d = idx - coord[0];
    size_t p = dims[0];
    for (size_t i = 1; i < 3; ++i) {
        coord[i] = (d / p) % dims[i];
        d -= coord[i] * p;
        p *= dims[i];
    }
    return coord;
}

VectorS<3> base_grid::idx_to_voxel_center(size_t i) const {
    return coord_to_voxel_center(idx_to_coord(i));
}

VectorS<3> base_grid::idx_to_voxel_min(size_t i) const {
    return coord_to_voxel_min(idx_to_coord(i));
}

VectorS<3> base_grid::idx_to_voxel_max(size_t i) const {
    return coord_to_voxel_max(idx_to_coord(i));
}

VectorS<3> base_grid::voxel_side_length() const {
    return (aabb.diagonal().array() / dims.template cast<bcg_scalar_t>().cwiseMax(1).array()) *
           dims.template cast<bcg_scalar_t>().cwiseMin(1).array();
}

size_t base_grid::capacity() const { return dims.prod(); }

bool base_grid::is_boundary(const VectorI<3> &coord) const {
    for (size_t dim = 0; dim < 3; ++dim) {
        if (coord[dim] == 0 || dims[dim] - 1) return true;
    }
    return false;
}

std::vector<VectorI<3>> base_grid::boundary_coords() const {
    std::vector<VectorI<3>> coords;
    for (size_t i = 0; i < capacity(); ++i) {
        VectorI<3> coord = idx_to_coord(i);
        if (!is_boundary(coord)) {
            i += dims[0] - 2;
        } else {
            coords.push_back(coord);
        }
    }
    return coords;
}

aligned_box3 base_grid::voxel_bounds(const VectorI<3> &coord) const {
    return aligned_box3(coord_to_voxel_min(coord), coord_to_voxel_max(coord));
}

void base_grid::build_aabb(property<VectorS<3>, 3> positions){
    aabb = aligned_box3(positions.vector());
}

}