//
// Created by alex on 07.11.20.
//

#ifndef BCG_GRAPHICS_KDTREE_H
#define BCG_GRAPHICS_KDTREE_H

#include <memory>
#include "nanoflann.hpp"
#include "math/bcg_linalg.h"
#include "bcg_neighbors_query.h"
#include "bcg_property.h"

namespace bcg {

template<int M, int D, typename Real>
using kdtree_matrix = nanoflann::KDTreeEigenMatrixAdaptor<Matrix<Real, M, D> >;

template<typename Real, int M = -1, int D = 3, class Distance = nanoflann::metric_L2>
struct kdtree_property {
    typedef kdtree_property<Real, M, D, Distance> self_t;
    typedef typename Distance::template traits<Real, self_t>::distance_t metric_t;
    typedef nanoflann::KDTreeSingleIndexAdaptor<metric_t, self_t, D, bcg_index_t> index_t;

    std::shared_ptr<index_t> index;
    property<VectorS<D>, D> positions;

    kdtree_property() = default;;

    explicit kdtree_property(property<VectorS<D>, D> positions, const int leaf_max_size = 10) : positions(positions) {
        index = std::make_shared<index_t>(static_cast<int>(D), *this,
                                          nanoflann::KDTreeSingleIndexAdaptorParams(leaf_max_size));
        index->buildIndex();
    }

    void build(property<VectorS<D>, D> positions, const int leaf_max_size = 10) {
        this->positions = positions;
        index = std::make_shared<index_t>(static_cast<int>(D), *this,
                                          nanoflann::KDTreeSingleIndexAdaptorParams(leaf_max_size));
        index->buildIndex();
    }

    inline size_t kdtree_get_point_count() const {
        return positions.size();
    }

    inline Real kdtree_get_pt(const size_t idx, const size_t dim) const {
        return positions[idx][dim];
    }

    template<class BBOX>
    bool kdtree_get_bbox(BBOX &) const {
        return false;
    }

    inline neighbors_query query_knn(const VectorS<D> &query_point, const size_t num_closest) const {
        neighbors_query result(num_closest);
        nanoflann::KNNResultSet<Real, bcg_index_t> resultSet(num_closest);
        resultSet.init(result.indices.data(), result.distances.data());
        index->findNeighbors(resultSet, query_point.data(), nanoflann::SearchParams());
        return result;
    }
};

}

#endif //BCG_GRAPHICS_KDTREE_H
