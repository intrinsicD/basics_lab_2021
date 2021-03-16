//
// Created by alex on 07.11.20.
//

#ifndef BCG_GRAPHICS_BCG_KDTREE_H
#define BCG_GRAPHICS_BCG_KDTREE_H

#include <memory>
#include <nanoflann.hpp>
#include "math/bcg_linalg.h"
#include "bcg_neighbors_query.h"
#include "bcg_property.h"
#include "utils/bcg_stl_utils.h"

namespace bcg {

template<typename Real, int M = -1, int D = 3, class Distance = nanoflann::metric_L2>
struct kdtree_matrix {
    struct DatasetAdaptor {
        DatasetAdaptor() = default;

        explicit DatasetAdaptor(const Matrix<Real, M, D> &positions) : positions(positions) {}

        const Matrix<Real, M, D> &positions;

        [[nodiscard]] inline size_t kdtree_get_point_count() const {
            return positions.rows();
        }

        //TODO somehow the shared pointer gets lost. Figure out why!
        inline Real kdtree_get_pt(const size_t idx, const size_t dim) const {
            return positions.row(idx)[dim];
        }

        template<class BBOX>
        bool kdtree_get_bbox(BBOX &) const {
            return false;
        }
    };

    typedef typename Distance::template traits<Real, DatasetAdaptor>::distance_t metric_t;
    typedef nanoflann::KDTreeSingleIndexAdaptor<metric_t, DatasetAdaptor, D, bcg_index_t> index_t;

    std::shared_ptr<index_t> index;
    std::shared_ptr<DatasetAdaptor> dataset;
    int leaf_size;

    kdtree_matrix() = default;

    explicit kdtree_matrix(const Matrix<Real, M, D> &positions, int leaf_max_size = 10) : dataset(
            std::make_shared<DatasetAdaptor>(positions)), leaf_size(leaf_max_size) {
        index = std::make_shared<index_t>(static_cast<int>(positions.cols()), *dataset,
                                          nanoflann::KDTreeSingleIndexAdaptorParams(leaf_max_size));
        index->buildIndex();
    }

    void build(const Matrix<Real, M, D> &positions, int leaf_max_size = 10) {
        dataset = std::make_shared<DatasetAdaptor>(positions);
        index = std::make_shared<index_t>(static_cast<int>(positions.cols()), *dataset,
                                          nanoflann::KDTreeSingleIndexAdaptorParams(leaf_max_size));
        index->buildIndex();
    }

    inline neighbors_query query_knn(const VectorS<D> &query_point, const size_t num_closest) const {
        neighbors_query result(num_closest);
        nanoflann::KNNResultSet<Real, bcg_index_t> resultSet(num_closest);
        resultSet.init(result.indices.data(), result.distances.data());
        index->findNeighbors(resultSet, query_point.data(), nanoflann::SearchParams());
        return result;
    }

    inline neighbors_query query_radius(const VectorS<D> &query_point, const bcg_scalar_t radius) const {
        neighbors_query result;
        std::vector<std::pair<bcg_index_t, bcg_scalar_t>> items;
        nanoflann::RadiusResultSet<Real, bcg_index_t> resultSet(radius, items);
        resultSet.init();
        index->findNeighbors(resultSet, query_point.data(), nanoflann::SearchParams());
        unzip(items, &result.indices, &result.distances);
        return result;
    }
};

template<typename Real, int M = -1, int D = 3, class Distance = nanoflann::metric_L2>
struct kdtree_property {
    struct DatasetAdaptor {
        DatasetAdaptor() = default;

        explicit DatasetAdaptor(property<VectorS<D>, D> positions) : positions(positions) {}

        property<VectorS<D>, D> positions;

        [[nodiscard]] inline size_t kdtree_get_point_count() const {
            return positions.size();
        }

        //TODO somehow the shared pointer gets lost. Figure out why!
        inline Real kdtree_get_pt(const size_t idx, const size_t dim) const {
            return positions[idx][dim];
        }

        template<class BBOX>
        bool kdtree_get_bbox(BBOX &) const {
            return false;
        }
    };

    typedef typename Distance::template traits<Real, DatasetAdaptor>::distance_t metric_t;
    typedef nanoflann::KDTreeSingleIndexAdaptor<metric_t, DatasetAdaptor, D, bcg_index_t> index_t;

    std::shared_ptr<index_t> index;
    std::shared_ptr<DatasetAdaptor> dataset;
    int leaf_size;

    kdtree_property() = default;

    explicit kdtree_property(property<VectorS<D>, D> positions, int leaf_max_size = 10) : dataset(
            std::make_shared<DatasetAdaptor>(positions)), leaf_size(leaf_max_size) {
        index = std::make_shared<index_t>(static_cast<int>(D), *dataset,
                                          nanoflann::KDTreeSingleIndexAdaptorParams(leaf_max_size));
        index->buildIndex();
    }

    void build(const property<VectorS<D>, D> &positions, int leaf_max_size = 10) {
        dataset = std::make_shared<DatasetAdaptor>(positions);
        index = std::make_shared<index_t>(static_cast<int>(D), *dataset,
                                          nanoflann::KDTreeSingleIndexAdaptorParams(leaf_max_size));
        index->buildIndex();
    }

    inline neighbors_query query_knn(const VectorS<D> &query_point, const size_t num_closest) const {
        neighbors_query result(num_closest);
        nanoflann::KNNResultSet<Real, bcg_index_t> resultSet(num_closest);
        resultSet.init(result.indices.data(), result.distances.data());
        index->findNeighbors(resultSet, query_point.data(), nanoflann::SearchParams());
        return result;
    }

    inline neighbors_query query_radius(const VectorS<D> &query_point, const bcg_scalar_t radius) const {
        neighbors_query result;
        std::vector<std::pair<bcg_index_t, bcg_scalar_t>> items;
        nanoflann::RadiusResultSet<Real, bcg_index_t> resultSet(radius, items);
        resultSet.init();
        index->findNeighbors(resultSet, query_point.data(), nanoflann::SearchParams());
        unzip(items, &result.indices, &result.distances);
        return result;
    }
};

}

#endif //BCG_GRAPHICS_BCG_KDTREE_H
