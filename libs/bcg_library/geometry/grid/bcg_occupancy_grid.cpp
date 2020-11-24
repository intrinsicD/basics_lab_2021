//
// Created by alex on 24.11.20.
//

#include "bcg_occupancy_grid.h"

namespace bcg {

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
        if (coord[i] < dims[i] || coord[i] > dims[i]) {
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
    return occupied.size();
}

size_t occupancy_grid::capacity() const {
    return dims.prod();
}

void occupancy_grid::clear() {
    occupied.clear();
}

}