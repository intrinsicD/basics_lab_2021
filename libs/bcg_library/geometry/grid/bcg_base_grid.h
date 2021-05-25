//
// Created by alex on 24.11.20.
//

#ifndef BCG_GRAPHICS_BCG_BASE_GRID_H
#define BCG_GRAPHICS_BCG_BASE_GRID_H

#include "geometry/aligned_box/bcg_aligned_box.h"
#include "geometry/bcg_property.h"

namespace bcg {

template<int D>
struct base_grid {
    base_grid() = default;

    base_grid(const VectorI<D> &dims, const aligned_box<D> &aabb) : dims(dims), aabb(aabb){
        vsl = voxel_side_length();
        aabb_diagonal = aabb.diagonal();
    }

    virtual ~base_grid() = default;

    size_t to_idx(const VectorS<D> &item) const{
        return coord_to_idx(to_coord(item));
    }

    VectorI<D> to_coord(const VectorS<D> &item) const{
        return (dims.template cast<bcg_scalar_t>().array() *
                ((item - aabb.min).array() / aabb_diagonal.array())).template cast<bcg_index_t>().cwiseMin(
                dims.array() - 1).cwiseMax(0);
    }

    VectorS<D> to_voxel_center(const VectorS<D> &item) const{
        return coord_to_voxel_center(to_coord(item));
    }

    VectorS<D> to_voxel_min(const VectorS<D> &item) const{
        return coord_to_voxel_min(to_coord(item));
    }

    VectorS<D> to_voxel_max(const VectorS<D> &item) const{
        return coord_to_voxel_max(to_coord(item));
    }

    size_t coord_to_idx(const VectorI<D> &coord) const{
        size_t result = coord[0];
        size_t prod = dims[0];
        for (size_t i = 1; i < dims.size(); ++i) {
            result += coord[i] * prod;
            prod *= dims[i];
        }
        return result;
    }

    VectorS<D> coord_to_voxel_center(const VectorI<D> &coord) const{
        return coord_to_voxel_min(coord) + vsl / 2;
    }

    VectorS<D> coord_to_voxel_min(const VectorI<D> &coord) const{
        return aabb.min.array() + coord.template cast<bcg_scalar_t>().array() * vsl.array();
    }

    VectorS<D> coord_to_voxel_max(const VectorI<D> &coord) const{
        return coord_to_voxel_min(coord) + vsl;
    }

    VectorI<D> idx_to_coord(size_t idx) const{
        VectorI<D> coord;
        coord[0] = idx % dims[0];
        size_t d = idx - coord[0];
        size_t p = dims[0];
        for (size_t i = 1; i < dims.size(); ++i) {
            coord[i] = (d / p) % dims[i];
            d -= coord[i] * p;
            p *= dims[i];
        }
        return coord;
    }

    VectorS<D> idx_to_voxel_center(size_t i) const{
        return coord_to_voxel_center(idx_to_coord(i));
    }

    VectorS<D> idx_to_voxel_min(size_t i) const{
        return coord_to_voxel_min(idx_to_coord(i));
    }

    VectorS<D> idx_to_voxel_max(size_t i) const{
        return coord_to_voxel_max(idx_to_coord(i));
    }

    VectorS<D> voxel_side_length() const{
        return (aabb_diagonal.array() / dims.template cast<bcg_scalar_t>().cwiseMax(1).array()) *
               dims.template cast<bcg_scalar_t>().cwiseMin(1).array();
    }

    [[nodiscard]] size_t capacity() const{
        return dims.prod();
    }

    bool is_boundary(const VectorI<D> &coord) const{
        for (size_t dim = 0; dim < dims.size(); ++dim) {
            if (coord[dim] == 0 || dims[dim] - 1) return true;
        }
        return false;
    }

    std::vector<VectorI<D>> boundary_coords() const{
        std::vector<VectorI<D>> coords;
        for (size_t i = 0; i < capacity(); ++i) {
            VectorI<D> coord = idx_to_coord(i);
            if (!is_boundary(coord)) {
                i += dims[0] - 2;
            } else {
                coords.push_back(coord);
            }
        }
        return coords;
    }

    aligned_box<D> voxel_bounds(const VectorI<D> &coord) const{
        auto min = coord_to_voxel_min(coord);
        return aligned_box<D>(min, min + vsl);
    }

    void build_aabb(property<VectorS<D>, D> positions){
        aabb = aligned_box<D>(positions.vector());
    }

    VectorI<D> dims;
    aligned_box<D> aabb;
    VectorS<D> vsl, aabb_diagonal;
};

template<int D>
inline std::ostream &operator<<(std::ostream &stream, const base_grid<D> &grid) {
    stream << grid.aabb << "\n";
    stream << "dims: " << grid.dims.transpose() << "\n";
    stream << "capacity: " << grid.capacity() << "\n";
    stream << "voxel_side_length: " << grid.vsl.transpose() << "\n";
    stream << "aabb_diagonal: " << grid.aabb_diagonal.transpose() << "\n";
    return stream;
}

using base_grid2 = base_grid<2>;

using base_grid3 = base_grid<3>;

}

#endif //BCG_GRAPHICS_BCG_BASE_GRID_H
