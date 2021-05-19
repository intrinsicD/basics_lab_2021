//
// Created by alex on 07.11.20.
//

#ifndef BCG_GRAPHICS_BCG_NEIGHBORS_QUERY_H
#define BCG_GRAPHICS_BCG_NEIGHBORS_QUERY_H

#include <vector>
#include "math/bcg_math_types.h"

namespace bcg {

struct neighbors_query {
    neighbors_query() = default;

    explicit neighbors_query(int k) : indices(k, 0), distances(k, 0) {}

    explicit neighbors_query(size_t k) : indices(k, 0), distances(k, 0) {}

    std::vector<bcg_index_t> indices;
    std::vector<bcg_scalar_t> distances;

    size_t size() const {
        return indices.size();
    }
};

}

#endif //BCG_GRAPHICS_BCG_NEIGHBORS_QUERY_H
