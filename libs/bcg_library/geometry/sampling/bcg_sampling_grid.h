//
// Created by alex on 08.02.21.
//

#ifndef BCG_GRAPHICS_BCG_SAMPLING_GRID_H
#define BCG_GRAPHICS_BCG_SAMPLING_GRID_H

#include <unordered_map>
#include "geometry/grid/bcg_occupancy_grid.h"

namespace bcg{

struct sampling_grid : private occupancy_grid{
    enum class SamplingType{
        first,
        last,
        mean,
        closest
        __last__
    }sampling_type;

    sampling_grid() = default;

    void insert_point(const VectorS<3> &point);

    std::vector<VectorS<3>> get_occupied_samples() const;

    void clear();

    vertex_container nodes;
    std::unordered_map<size_t, size_t> sparse_map;
};

}

#endif //BCG_GRAPHICS_BCG_SAMPLING_GRID_H
