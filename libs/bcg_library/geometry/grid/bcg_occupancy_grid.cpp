//
// Created by alex on 24.11.20.
//

#include "bcg_occupancy_grid.h"
#include "utils/bcg_stl_utils.h"
#include "tbb/tbb.h"

namespace bcg {

occupancy_grid::occupancy_grid(const VectorI<3> &dims, const aligned_box3 &aabb) : base_grid(dims, aabb){

}

void occupancy_grid::build(property<VectorS<3>, 3> positions, const VectorI<3> &dims) {
    build_aabb(positions);
    this->dims = dims;
    for(size_t i = 0; i < positions.size(); ++i){
        mark_occupied(positions[i]);
    }
}

bool occupancy_grid::is_occupied(const VectorS<3> &point) const {
    return occupied.test(to_idx(point));
}

bool occupancy_grid::is_occupied(const VectorI<3> &coord) const {
    return occupied.test(coord_to_idx(coord));
}

bool occupancy_grid::is_occupied(size_t idx) const {
    return occupied.test(idx);
}

bool occupancy_grid::is_inside_bounds(const VectorS<3> &point) const {
    for (size_t i = 0; i < 3; ++i) {
        if (point[i] < aabb.min[i] || point[i] > aabb.max[i]) {
            return false;
        }
    }
    return true;
}

bool occupancy_grid::is_inside_bounds(const VectorI<3> &coord) const {
    for (size_t i = 0; i < 3; ++i) {
        if (coord[i] < 0 || coord[i] > dims[i]) {
            return false;
        }
    }
    return true;
}

bool occupancy_grid::is_inside_bounds(size_t idx) const {
    return idx < capacity();
}

void occupancy_grid::mark_occupied(const VectorS<3> &point) {
    auto idx = to_idx(point);
    if(is_inside_bounds(idx)){
        occupied.set(idx);
    }
}

void occupancy_grid::mark_occupied(const VectorI<3> &coord) {
    auto idx = coord_to_idx(coord);
    if(is_inside_bounds(idx)){
        occupied.set(idx);
    }
}

void occupancy_grid::mark_occupied(size_t idx) {
    if(is_inside_bounds(idx)){
        occupied.set(idx);
    }
}

void occupancy_grid::mark_free(const VectorS<3> &point) {
    auto idx = to_idx(point);
    if(is_inside_bounds(idx)){
        occupied.reset(idx);
    }
}

void occupancy_grid::mark_free(const VectorI<3> &coord) {
    auto idx = coord_to_idx(coord);
    if(is_inside_bounds(idx)){
        occupied.reset(idx);
    }
}

void occupancy_grid::mark_free(size_t idx) {
    if(is_inside_bounds(idx)){
        occupied.reset(idx);
    }
}

size_t occupancy_grid::num_free_voxel() const {
    return dims.prod() - occupied.size();
}

size_t occupancy_grid::num_occupied_voxel() const {
    return occupied.count();
}

size_t occupancy_grid::capacity() const {
    return dims.prod();
}

void occupancy_grid::clear() {
    occupied.clear();
}
void traverse(occupancy_grid &grid, const std::function<bool(size_t, occupancy_grid &)> &function){
    for (size_t i = 0; i < grid.capacity(); ++i) {
        if (!function(i, grid)) return;
    }
}

void traverse_parallel(occupancy_grid &grid, const std::function<bool(size_t, occupancy_grid &)> &function){
    tbb::parallel_for(
            tbb::blocked_range<uint32_t>(0u, (uint32_t) grid.capacity(), 1024),
            [&](const tbb::blocked_range<uint32_t> &range) {
                for (uint32_t i = range.begin(); i != range.end(); ++i) {
                    if (!function(i, grid)) return;
                }
            }
    );
}

neighbors_query query_kring(const occupancy_grid &grid, const VectorS<3> &query_point, size_t k){
    occupancy_grid moore(VectorI<3>::Constant(2 * k + 1), grid.aabb);
    VectorI<3> center_coord = moore.to_coord(grid.aabb.center());
    VectorI<3> query_coord = grid.to_coord(query_point);
    neighbors_query result;
    traverse(moore, [&query_point, &query_coord, &center_coord, &grid, &result](size_t idx,
                                                                                         const occupancy_grid &moore) {
        VectorI<3> current_coords = query_coord + moore.idx_to_coord(idx) - center_coord;
        if (grid.is_inside_bounds(current_coords) && grid.is_occupied(current_coords)) {
            auto distance = (query_point - grid.coord_to_voxel_center(current_coords)).norm();
            result.indices.push_back(grid.coord_to_idx(current_coords));
            result.distances.push_back(distance);
        }
        return true;
    });

    return result;
}

neighbors_query query_knn(const occupancy_grid &grid, const VectorS<3> &query_point, size_t num_closest){
    VectorI<3> query_coord = grid.to_coord(query_point);
    neighbors_query result_set;
    int kring = 1;
    occupancy_grid boundary({2, 2, 2}, grid.aabb);
    VectorI<3> center_coord = boundary.to_coord(grid.aabb.center());

    while (result_set.indices.size() < num_closest){
        if (kring >= grid.dims.minCoeff()) break;
        boundary.dims = VectorI<3>::Constant(2 * kring + 1);
        for (const VectorI<3> &offset : boundary.boundary_coords()) {
            VectorI<3> current_coords = query_coord + offset - center_coord;
            if (grid.is_inside_bounds(current_coords) && grid.is_occupied(current_coords)) {
                auto distance = (query_point - grid.coord_to_voxel_center(current_coords)).norm();
                result_set.indices.push_back(grid.coord_to_idx(current_coords));
                result_set.distances.push_back(distance);
            }
        }

        sort_by_first(result_set.distances, result_set.indices);
        ++kring;
    }
    result_set.indices.resize(num_closest);
    result_set.distances.resize(num_closest);
    return result_set;
}

neighbors_query query_radius(const occupancy_grid &grid, const VectorS<3> &query_point, bcg_scalar_t radius){
    VectorI<3> query_coord = grid.to_coord(query_point);
    neighbors_query result_set;
    int kring = fmax(radius / grid.voxel_side_length().maxCoeff(), bcg_scalar_t (1));
    occupancy_grid moore(VectorI<3>::Constant(2 * kring + 1), grid.aabb);
    VectorI<3> center_coord = moore.to_coord(grid.aabb.center());

    traverse(moore, [&query_point, &query_coord, &center_coord, &grid, &result_set, &radius](size_t idx,
                                                                                                  const occupancy_grid &moore) {
        VectorI<3> current_coords = query_coord + moore.idx_to_coord(idx) - center_coord;
        if (grid.is_inside_bounds(current_coords) && grid.is_occupied(current_coords)) {
            auto distance = (query_point - grid.coord_to_voxel_center(current_coords)).norm();
            if (distance <= radius) {
                result_set.indices.push_back(grid.coord_to_idx(current_coords));
                result_set.distances.push_back(distance);
            }
        }
        return true;
    });

    return result_set;
}




}