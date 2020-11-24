//
// Created by alex on 24.11.20.
//

#include "bcg_voxel_grid.h"

namespace bcg{

void voxel_grid::build(property<VectorS<3>, 3> positions, const VectorI<3> &dims){
    build_aabb(positions);
    this->dims = dims;
    successor = std::vector<size_t>(positions.size());
    for(size_t i = 0; i < positions.size(); ++i){
        successor[i] = i + 1;
    }
    for(size_t i = 0; i < positions.size(); ++i){
        insert(positions[i], vertex_handle(i));
    }
}

bool voxel_grid::is_occupied(const VectorS<3> &point) const{
    return occupancy_grid::is_occupied(point);
}

bool voxel_grid::is_occupied(const VectorI<3> &coord) const{
    return occupancy_grid::is_occupied(coord);
}

bool voxel_grid::is_occupied(size_t idx) const{
    return occupancy_grid::is_occupied(idx);
}

bool voxel_grid::is_inside_bounds(const VectorS<3> &point) const{
    return occupancy_grid::is_inside_bounds(point);
}

bool voxel_grid::is_inside_bounds(const VectorI<3> &coord) const{
    return occupancy_grid::is_inside_bounds(coord);
}

bool voxel_grid::is_inside_bounds(size_t idx) const{
    return occupancy_grid::is_inside_bounds(idx);
}

void voxel_grid::insert(const VectorS<3> &point, vertex_handle v){
    insert(to_idx(point), v);
}

void voxel_grid::insert(const VectorI<3> &coord, vertex_handle v){
    insert(coord_to_idx(coord), v);
}

void voxel_grid::insert(size_t idx, vertex_handle v){
    if(!is_inside_bounds(idx)) return;
    if(!is_occupied(idx)){
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