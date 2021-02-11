//
// Created by alex on 09.02.21.
//

#include "bcg_sampling_grid.h"
#include "bcg_property_map_eigen.h"

namespace bcg {

std::vector<std::string> grid_sampling_type_names(){
    std::vector<std::string> names(static_cast<int>(GridSamplingType::__last__));
    names[static_cast<int>(GridSamplingType::first)] = "first";
    names[static_cast<int>(GridSamplingType::last)] = "last";
    names[static_cast<int>(GridSamplingType::closest)] = "closest";
    names[static_cast<int>(GridSamplingType::mean)] = "mean";
    return names;
}

/*----------------------------------------------------------------------------------------------------------------------
 *  3D Sampling grid first:
 *      - dims and aabb in base class.
 *      - using build() functions to iterate over all points in
 *          - property vector : capture without copy
 *          - vector:  capture with copy
 *          - eigen matrix Nx3: capture with copy
 *      - selects the first point index which is inserted into each voxel
 *
 *--------------------------------------------------------------------------------------------------------------------*/

sample_first_grid::sample_first_grid(const VectorI<3> &dims, const aligned_box3 &aabb) : sampling_grid(dims, aabb),
                                                                                         sampled_index(
                                                                                                 nodes.get_or_add<size_t, 1>(
                                                                                                         "sampled_indices")) {

}

void sample_first_grid::build(property<VectorS<3>, 3> points) {
    ref_points = vertices.add(points);

    size_t size = ref_points.size();
    for (size_t i = 0; i < size; ++i) {
        insert_point(ref_points[i], i);
    }
}

void sample_first_grid::build(const std::vector<VectorS<3>> &points) {
    ref_points = vertices.get_or_add<VectorS<3>, 3>("ref_points");
    vertices.resize(points.size());
    ref_points.vector() = points;

    size_t size = ref_points.size();
    for (size_t i = 0; i < size; ++i) {
        insert_point(ref_points[i], i);
    }
}

void sample_first_grid::build(const MatrixS<-1, 3> &points) {
    ref_points = vertices.get_or_add<VectorS<3>, 3>("ref_points");
    vertices.resize(points.size());
    Map(ref_points) = points;

    size_t size = ref_points.size();
    for (size_t i = 0; i < size; ++i) {
        insert_point(ref_points[i], i);
    }
}

void sample_first_grid::insert_point(const VectorS<3> &point, size_t idx) {
    size_t lin_idx = to_idx(point);
    if (!is_occupied_idx(lin_idx)) {
        mark_occupied_idx(lin_idx);
        nodes.push_back();
        sampled_index[nodes.size() - 1] = idx;
        grid_map.emplace(lin_idx, nodes.size() - 1);
    }
}

std::vector<VectorS<3>> sample_first_grid::get_occupied_sample_points() const {
    std::vector<VectorS<3>> points(sampled_index.size());
    for (size_t i = 0; i < points.size(); ++i) {
        points[i] = ref_points[sampled_index[i]];
    }
    return points;
}

std::vector<size_t> sample_first_grid::get_occupied_samples_indices() const{
    return sampled_index.vector();
}

void sample_first_grid::clear() {
    grid_map.clear();
    sampled_index.clear();
    ref_points.clear();
    sampling_grid::occupancy_grid::clear();
}


/*----------------------------------------------------------------------------------------------------------------------
 *  3D Sampling grid last:
 *      - dims and aabb in base class.
 *      - using build() functions to iterate over all points in
 *          - property vector : capture without copy
 *          - vector:  capture with copy
 *          - eigen matrix Nx3: capture with copy
 *      - selects the last point index which is inserted into each voxel
 *
 *--------------------------------------------------------------------------------------------------------------------*/

sample_last_grid::sample_last_grid(const VectorI<3> &dims, const aligned_box3 &aabb) : sampling_grid(dims, aabb),
                                                                                       sampled_index(
                                                                                               nodes.get_or_add<size_t, 1>(
                                                                                                       "sampled_indices")) {

}

void sample_last_grid::build(property<VectorS<3>, 3> points) {
    ref_points = vertices.add(points);

    size_t size = ref_points.size();
    for (size_t i = 0; i < size; ++i) {
        insert_point(ref_points[i], i);
    }
}

void sample_last_grid::build(const std::vector<VectorS<3>> &points) {
    ref_points = vertices.get_or_add<VectorS<3>, 3>("ref_points");
    vertices.resize(points.size());
    ref_points.vector() = points;

    size_t size = ref_points.size();
    for (size_t i = 0; i < size; ++i) {
        insert_point(ref_points[i], i);
    }
}

void sample_last_grid::build(const MatrixS<-1, 3> &points) {
    ref_points = vertices.get_or_add<VectorS<3>, 3>("ref_points");
    vertices.resize(points.size());
    Map(ref_points) = points;

    size_t size = ref_points.size();
    for (size_t i = 0; i < size; ++i) {
        insert_point(ref_points[i], i);
    }
}

void sample_last_grid::insert_point(const VectorS<3> &point, size_t idx) {
    size_t lin_idx = to_idx(point);
    if (!is_occupied_idx(lin_idx)) {
        mark_occupied_idx(lin_idx);
        nodes.push_back();
        sampled_index[nodes.size() - 1] = idx;
        grid_map.emplace(lin_idx, nodes.size() - 1);
    } else {
        sampled_index[grid_map[lin_idx]] = idx;
    }
}

std::vector<VectorS<3>> sample_last_grid::get_occupied_sample_points() const {
    std::vector<VectorS<3>> points(sampled_index.size());
    for (size_t i = 0; i < points.size(); ++i) {
        points[i] = ref_points[sampled_index[i]];
    }
    return points;
}

std::vector<size_t> sample_last_grid::get_occupied_samples_indices() const{
    return sampled_index.vector();
}

void sample_last_grid::clear() {
    grid_map.clear();
    sampled_index.clear();
    ref_points.clear();
    sampling_grid::occupancy_grid::clear();
}


/*----------------------------------------------------------------------------------------------------------------------
 *  3D Sampling grid closest:
 *      - dims and aabb in base class.
 *      - using build() functions to iterate over all points in
 *          - property vector : capture without copy
 *          - vector:  capture with copy
 *          - eigen matrix Nx3: capture with copy
 *      - selects the closest point index to the voxel center, which is inserted into each voxel
 *
 *--------------------------------------------------------------------------------------------------------------------*/

sample_closest_grid::sample_closest_grid(const VectorI<3> &dims, const aligned_box3 &aabb) : sampling_grid(dims, aabb),
                                                                                             sampled_index(
                                                                                                     nodes.get_or_add<size_t, 1>(
                                                                                                             "sampled_indices")),
                                                                                             distances_to_voxel_centers(
                                                                                                     nodes.get_or_add<bcg_scalar_t, 1>(
                                                                                                             "distances")) {

}

void sample_closest_grid::build(property<VectorS<3>, 3> points) {
    ref_points = vertices.add(points);

    size_t size = ref_points.size();
    for (size_t i = 0; i < size; ++i) {
        insert_point(ref_points[i], i);
    }
}

void sample_closest_grid::build(const std::vector<VectorS<3>> &points) {
    ref_points = vertices.get_or_add<VectorS<3>, 3>("ref_points");
    vertices.resize(points.size());
    ref_points.vector() = points;

    size_t size = ref_points.size();
    for (size_t i = 0; i < size; ++i) {
        insert_point(ref_points[i], i);
    }
}

void sample_closest_grid::build(const MatrixS<-1, 3> &points) {
    ref_points = vertices.get_or_add<VectorS<3>, 3>("ref_points");
    vertices.resize(points.size());
    Map(ref_points) = points;

    size_t size = ref_points.size();
    for (size_t i = 0; i < size; ++i) {
        insert_point(ref_points[i], i);
    }
}

void sample_closest_grid::insert_point(const VectorS<3> &point, size_t idx) {
    size_t lin_idx = to_idx(point);
    if (!is_occupied_idx(lin_idx)) {
        mark_occupied_idx(lin_idx);
        nodes.push_back();
        sampled_index[nodes.size() - 1] = idx;
        grid_map.emplace(lin_idx, nodes.size() - 1);
        distances_to_voxel_centers[nodes.size() - 1] = (idx_to_voxel_center(lin_idx) - point).norm();
    } else {
        bcg_scalar_t distance = (idx_to_voxel_center(lin_idx) - point).norm();
        size_t voxel_idx = grid_map[lin_idx];
        if (distance < distances_to_voxel_centers[voxel_idx]) {
            sampled_index[voxel_idx] = idx;
            distances_to_voxel_centers[voxel_idx] = distance;
        }
    }
}

std::vector<VectorS<3>> sample_closest_grid::get_occupied_sample_points() const {
    std::vector<VectorS<3>> points(sampled_index.size());
    for (size_t i = 0; i < points.size(); ++i) {
        points[i] = ref_points[sampled_index[i]];
    }
    return points;
}

std::vector<size_t> sample_closest_grid::get_occupied_samples_indices() const{
    return sampled_index.vector();
}

void sample_closest_grid::clear() {
    grid_map.clear();
    sampled_index.clear();
    ref_points.clear();
    sampling_grid::occupancy_grid::clear();
}


/*----------------------------------------------------------------------------------------------------------------------
 *  3D Sampling grid mean:
 *      - dims and aabb in base class.
 *      - using build() functions to iterate over all points in
 *          - property vector : capture without copy
 *          - vector:  capture with copy
 *          - eigen matrix Nx3: capture with copy
 *      - compute the mean of the points in each voxel
 *
 *--------------------------------------------------------------------------------------------------------------------*/

sample_mean_grid::sample_mean_grid(const VectorI<3> &dims, const aligned_box3 &aabb) : sampling_grid(dims, aabb),
                                                                                       sample_points(
                                                                                               nodes.get_or_add<VectorS<3>, 3>(
                                                                                                       "samples")),
                                                                                       sample_size(
                                                                                               nodes.get_or_add<size_t, 1>(
                                                                                                       "sample_size")) {

}

void sample_mean_grid::build(property<VectorS<3>, 3> points) {
    ref_points = vertices.add(points);

    size_t size = ref_points.size();
    for (size_t i = 0; i < size; ++i) {
        insert_point(ref_points[i]);
    }
}

void sample_mean_grid::build(const std::vector<VectorS<3>> &points) {
    ref_points = vertices.get_or_add<VectorS<3>, 3>("ref_points");
    vertices.resize(points.size());
    ref_points.vector() = points;

    size_t size = ref_points.size();
    for (size_t i = 0; i < size; ++i) {
        insert_point(ref_points[i]);
    }
}

void sample_mean_grid::build(const MatrixS<-1, 3> &points) {
    ref_points = vertices.get_or_add<VectorS<3>, 3>("ref_points");
    vertices.resize(points.size());
    Map(ref_points) = points;

    size_t size = ref_points.size();
    for (size_t i = 0; i < size; ++i) {
        insert_point(ref_points[i]);
    }
}

void sample_mean_grid::insert_point(const VectorS<3> &point) {
    size_t lin_idx = to_idx(point);
    if (!is_occupied_idx(lin_idx)) {
        mark_occupied_idx(lin_idx);
        nodes.push_back();
        size_t voxel_idx = nodes.size() - 1;
        sample_points[voxel_idx] = point;
        sample_size[voxel_idx] = 1;
        grid_map.emplace(lin_idx, voxel_idx);
    } else {
        size_t voxel_idx = grid_map[lin_idx];
        sample_points[voxel_idx] = sample_points[voxel_idx] * sample_size[voxel_idx] + point;
        ++sample_size[voxel_idx];
        sample_points[voxel_idx] /= sample_size[voxel_idx];
    }
}

std::vector<VectorS<3>> sample_mean_grid::get_occupied_sample_points() const {
    return sample_points.vector();
}

void sample_mean_grid::clear() {
    grid_map.clear();
    sample_points.clear();
    ref_points.clear();
    sampling_grid::occupancy_grid::clear();
}


}