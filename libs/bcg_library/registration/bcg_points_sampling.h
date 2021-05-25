//
// Created by alex on 25.05.21.
//

#ifndef BCG_GRAPHICS_BCG_POINTS_SAMPLING_H
#define BCG_GRAPHICS_BCG_POINTS_SAMPLING_H

#include <vector>
#include "geometry/grid/bcg_occupancy_grid.h"
#include "math/bcg_linalg.h"
#include "point_cloud/bcg_point_cloud.h"

namespace bcg {

struct points_sampling {
    points_sampling() = default;

    points_sampling(vertex_container *vertices);

    virtual ~points_sampling() = default;

    void clear();

    virtual void sample(){}

    virtual MatrixS<-1, -1> embedding(const MatrixS<-1, -1> &data) const;

    vertex_container *vertices;
    std::vector<size_t> sampled_indices;
};

struct points_sampling_identity : public points_sampling {
    explicit points_sampling_identity(vertex_container *vertices);

    ~points_sampling_identity() override = default;

    void sample() override;
};

struct points_sampling_random : public points_sampling_identity {
    explicit points_sampling_random(vertex_container *vertices, size_t num_samples);

    ~points_sampling_identity() override = default;

    void sample() override;

    void sample(size_t num_samples);

    size_t num_samples;
};

struct points_sampling_grid : public points_sampling_identity {
    explicit points_sampling_grid(vertex_container *vertices);

    ~points_sampling_identity() override = default;

    void sample() override;

    void sample(size_t num_samples);

    VectorI<-1> dims;
};

}

#endif //BCG_GRAPHICS_BCG_POINTS_SAMPLING_H
