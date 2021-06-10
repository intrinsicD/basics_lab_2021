//
// Created by alex on 24.11.20.
//

#ifndef BCG_GRAPHICS_BCG_OCCUPANCY_GRID_H
#define BCG_GRAPHICS_BCG_OCCUPANCY_GRID_H

#include <vector>
#include "bcg_property.h"
#include "bcg_base_nd_grid.h"
#include "bcg_neighbors_query.h"
#include "utils/bcg_dynamic_bitset.h"
#include "utils/bcg_stl_utils.h"

namespace bcg {

template<int D>
struct occupancy_grid : public base_grid<D> {
    occupancy_grid() = default;

    occupancy_grid(const VectorI<D> &dims, const aligned_box<D> &aabb) : base_grid<D>(dims, aabb){

    }

    ~occupancy_grid() override = default;

    virtual void build(property<VectorS<D>, D> positions){
        for (size_t i = 0; i < positions.size(); ++i) {
            mark_occupied_point(positions[i]);
        }
    }

    virtual void build(const std::vector<VectorS<D>> &positions){
        for (const VectorS<D> &position : positions) {
            mark_occupied_point(position);
        }
    }

    virtual void build(const MatrixS<-1, D> &positions){
        for (auto i = 0; i < positions.rows(); ++i) {
            mark_occupied_point(positions.row(i));
        }
    }

    bool is_occupied_point(const VectorS<D> &point) const{
        return occupied.test(this->to_idx(point));
    }

    bool is_occupied_coord(const VectorI<D> &coord) const{
        return occupied.test(this->coord_to_idx(coord));
    }

    [[nodiscard]] bool is_occupied_idx(size_t idx) const{
        return occupied.test(idx);
    }

    void mark_occupied_point(const VectorS<D> &point){
        mark_occupied_coord(this->to_coord(point));
    }

    void mark_occupied_coord(const VectorI<D> &coord){
        mark_occupied_idx(this->coord_to_idx(coord));
    }

    void mark_occupied_idx(size_t idx){
        if (this->is_inside_bounds_idx(idx)) {
            occupied.set(idx);
        }
    }

    void mark_free_point(const VectorS<D> &point){
        mark_free_coord(this->to_coord(point));
    }

    void mark_free_coord(const VectorI<D> &coord){
        mark_free_idx(this->coord_to_idx(coord));
    }

    void mark_free_idx(size_t idx){
        if (this->is_inside_bounds_idx(idx)) {
            occupied.reset(idx);
        }
    }

    [[nodiscard]] size_t num_free_voxel() const{
        return this->dims.prod() - occupied.size();
    }

    [[nodiscard]] size_t num_occupied_voxel() const{
        return occupied.count();
    }

    virtual void clear(){
        occupied.clear();
    }

    DynamicBitset occupied;
};

template<int D>
void traverse(occupancy_grid<D> &grid, const std::function<bool(size_t, occupancy_grid<D> &)> &function){
    for (size_t i = 0; i < grid.capacity(); ++i) {
        if(grid.is_occupied_idx(i)){
            function(i, grid);
        }
    }
}

template<int D>
neighbors_query query_kring(const occupancy_grid<D> &grid, const VectorS<D> &query_point, size_t k){
    occupancy_grid<D> moore(VectorI<D>::Constant(2 * k + 1), grid.aabb);
    VectorI<D> center_coord = moore.to_coord(grid.aabb.center());
    VectorI<D> query_coord = grid.to_coord(query_point);
    neighbors_query result;
    traverse<D>(moore, [&query_point, &query_coord, &center_coord, &grid, &result](size_t idx,
                                                                                const occupancy_grid<D> &moore) {
        VectorI<D> current_coords = query_coord + moore.idx_to_coord(idx) - center_coord;
        if (grid.is_inside_bounds_coord(current_coords) && grid.is_occupied_coord(current_coords)) {
            auto distance = (query_point - grid.coord_to_voxel_center(current_coords)).norm();
            result.indices.push_back(grid.coord_to_idx(current_coords));
            result.distances.push_back(distance);
        }
        return true;
    });

    return result;
}

template<int D>
neighbors_query query_knn(const occupancy_grid<D> &grid, const VectorS<D> &query_point, size_t num_closest){
    VectorI<D> query_coord = grid.to_coord(query_point);
    neighbors_query result_set;
    bcg_index_t kring = 1;
    occupancy_grid<D> boundary({2, 2, 2}, grid.aabb);
    VectorI<D> center_coord = boundary.to_coord(grid.aabb.center());

    while (result_set.indices.size() < num_closest) {
        if (kring >= grid.dims.minCoeff()) break;
        boundary.dims = VectorI<D>::Constant(2 * kring + 1);
        for (const VectorI<D> &offset : boundary.boundary_coords()) {
            VectorI<D> current_coords = query_coord + offset - center_coord;
            if (grid.is_inside_bounds_coord(current_coords) && grid.is_occupied_coord(current_coords)) {
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

template<int D>
neighbors_query query_radius(const occupancy_grid<D> &grid, const VectorS<D> &query_point, bcg_scalar_t radius){
    VectorI<D> query_coord = grid.to_coord(query_point);
    neighbors_query result_set;
    int kring = std::max<int>(radius / grid.voxel_side_length().maxCoeff(), 1);
    occupancy_grid<D> moore(VectorI<D>::Constant(2 * kring + 1), grid.aabb);
    VectorI<D> center_coord = moore.to_coord(grid.aabb.center());

    traverse<D>(moore, [&query_point, &query_coord, &center_coord, &grid, &result_set, &radius](size_t idx,
                                                                                             const occupancy_grid<D> &moore) {
        VectorI<D> current_coords = query_coord + moore.idx_to_coord(idx) - center_coord;
        if (grid.is_inside_bounds_coord(current_coords) && grid.is_occupied_coord(current_coords)) {
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

template<int D>
std::vector<VectorS<D>> get_vertices(const occupancy_grid<D> &grid){
    occupancy_grid<D> dummy;
    dummy.aabb = grid.aabb;
    VectorS<D> vsl = grid.voxel_side_length();
    dummy.aabb.max += vsl;
    dummy.dims = grid.dims + VectorI<D>::Ones();

    std::vector<VectorS<D>> vertices(dummy.dims.prod());
    for (size_t i = 0; i < dummy.capacity(); ++i) {
        vertices[i] = dummy.idx_to_voxel_center(i) - vsl;
    }
    return vertices;
}

template<int D>
std::vector<VectorI<2>> get_edges(const occupancy_grid<D> &grid);

using occupancy_grid2 = occupancy_grid<2>;
using occupancy_grid3 = occupancy_grid<3>;

}

#endif //BCG_GRAPHICS_BCG_OCCUPANCY_GRID_H
