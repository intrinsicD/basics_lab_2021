//
// Created by alex on 08.02.21.
//

#ifndef BCG_GRAPHICS_BCG_SAMPLING_GRID_H
#define BCG_GRAPHICS_BCG_SAMPLING_GRID_H

#include <unordered_map>
#include "geometry/grid/bcg_occupancy_grid.h"

namespace bcg{

enum class GridSamplingType{
    first,
    last,
    closest,
    mean,
    __last__
};

std::vector<std::string> grid_sampling_type_names();

struct sampling_grid : protected occupancy_grid{
    sampling_grid() = default;

    sampling_grid(const VectorI<3> &dims, const aligned_box3 &aabb) : occupancy_grid(dims, aabb) {};

    ~sampling_grid() override = default;

    virtual void build(property<VectorS<3>, 3> points) = 0;

    virtual void build(const std::vector<VectorS<3>> &points) = 0;

    virtual void build(const MatrixS<-1, 3> &points) = 0;

    virtual std::vector<VectorS<3>> get_occupied_sample_points() const = 0;

    vertex_container nodes;
    vertex_container vertices;
    std::unordered_map<size_t, size_t> grid_map;
};

struct sample_first_grid : public sampling_grid{
    sample_first_grid() = default;

    sample_first_grid(const VectorI<3> &dims, const aligned_box3 &aabb);

    ~sample_first_grid() override = default;

    void build(property<VectorS<3>, 3> points) override;

    void build(const std::vector<VectorS<3>> &points) override;

    void build(const MatrixS<-1, 3> &points) override;

    void insert_point(const VectorS<3> &point, size_t idx);

    std::vector<VectorS<3>> get_occupied_sample_points() const override;

    std::vector<size_t> get_occupied_samples_indices() const;

    void clear() override;

    property<VectorS<3>, 3> ref_points;
    property<size_t, 1> sampled_index;
};

struct sample_last_grid : public sampling_grid{
    sample_last_grid() = default;

    sample_last_grid(const VectorI<3> &dims, const aligned_box3 &aabb);

    ~sample_last_grid() override = default;

    void build(property<VectorS<3>, 3> points) override;

    void build(const std::vector<VectorS<3>> &points) override;

    void build(const MatrixS<-1, 3> &points) override;

    void insert_point(const VectorS<3> &point, size_t idx);

    std::vector<VectorS<3>> get_occupied_sample_points() const override;

    std::vector<size_t> get_occupied_samples_indices() const;

    void clear() override;

    property<VectorS<3>, 3> ref_points;
    property<size_t, 1> sampled_index;
};


struct sample_closest_grid : public sampling_grid{
    sample_closest_grid() = default;

    sample_closest_grid(const VectorI<3> &dims, const aligned_box3 &aabb);

    ~sample_closest_grid() override = default;

    void build(property<VectorS<3>, 3> points) override;

    void build(const std::vector<VectorS<3>> &points) override;

    void build(const MatrixS<-1, 3> &points) override;

    void insert_point(const VectorS<3> &point, size_t idx);

    std::vector<VectorS<3>> get_occupied_sample_points() const override;

    std::vector<size_t> get_occupied_samples_indices() const;

    void clear() override;

    property<VectorS<3>, 3> ref_points;
    property<size_t, 1> sampled_index;
    property<bcg_scalar_t, 1> distances_to_voxel_centers;
};

struct sample_mean_grid : public sampling_grid{
    sample_mean_grid() = default;

    sample_mean_grid(const VectorI<3> &dims, const aligned_box3 &aabb);

    ~sample_mean_grid() override = default;

    void build(property<VectorS<3>, 3> points) override;

    void build(const std::vector<VectorS<3>> &points) override;

    void build(const MatrixS<-1, 3> &points) override;

    void insert_point(const VectorS<3> &point);

    std::vector<VectorS<3>> get_occupied_sample_points() const override;

    void clear() override;

    property<VectorS<3>, 3> ref_points;
    property<VectorS<3>, 3> sample_points;
    property<size_t, 1> sample_size;
};

}

#endif //BCG_GRAPHICS_BCG_SAMPLING_GRID_H
