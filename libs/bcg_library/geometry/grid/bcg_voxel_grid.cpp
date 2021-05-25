//
// Created by alex on 24.11.20.
//

#include "bcg_voxel_grid.h"

namespace bcg{

void voxel_grid::build(property<VectorS<3>, 3> positions, const VectorI<3> &dims){
    build_aabb(positions.vector());
    this->dims = dims;
    successor = std::vector<size_t>(positions.size());
    for(size_t i = 0; i < positions.size(); ++i){
        successor[i] = i + 1;
    }
    for(size_t i = 0; i < positions.size(); ++i){
        insert_point(positions[i], vertex_handle(i));
    }
}

bool voxel_grid::is_occupied_point(const VectorS<3> &point) const {
    return occupancy_grid::is_occupied_point(point);
}

bool voxel_grid::is_occupied_coord(const VectorI<3> &coord) const{
    return occupancy_grid::is_occupied_coord(coord);
}

bool voxel_grid::is_occupied_idx(size_t idx) const{
    return occupancy_grid::is_occupied_idx(idx);
}

bool voxel_grid::is_inside_bounds_point(const VectorS<3> &point) const{
    return occupancy_grid::is_inside_bounds_point(point);
}

bool voxel_grid::is_inside_bounds_coord(const VectorI<3> &coord) const{
    return occupancy_grid::is_inside_bounds_coord(coord);
}

bool voxel_grid::is_inside_bounds_idx(size_t idx) const{
    return occupancy_grid::is_inside_bounds_idx(idx);
}

void voxel_grid::insert_point(const VectorS<3> &point, vertex_handle v){
    insert_idx(to_idx(point), v);
}

void voxel_grid::insert_coord(const VectorI<3> &coord, vertex_handle v){
    insert_idx(coord_to_idx(coord), v);
}

void voxel_grid::insert_idx(size_t idx, vertex_handle v){
    if(!is_inside_bounds_idx(idx)) return;
    if(!is_occupied_idx(idx)){
        storage[idx] = {v, v, 1};
    }else{
        successor[storage[idx].v_end] = v;
        storage[idx].v_end = v;
        ++storage[idx].size;
    }
}

size_t voxel_grid::num_free_voxel() const{
    return occupancy_grid::num_free_voxel();
}

size_t voxel_grid::num_occupied_voxel() const{
    return occupancy_grid::num_occupied_voxel();
}

size_t voxel_grid::capacity() const{
    return occupancy_grid::capacity();
}

void voxel_grid::clear(){
    occupancy_grid::clear();
    successor.clear();
    storage.clear();
}

}