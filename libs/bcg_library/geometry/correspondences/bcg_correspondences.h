//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_CORRESPONDENCES_H
#define BCG_GRAPHICS_BCG_CORRESPONDENCES_H

#include <unordered_map>
#include "point_cloud/bcg_point_cloud.h"
#include "math/sparse_matrix/bcg_sparse_matrix.h"

namespace bcg {

struct correspondences {
    using container_t = std::vector<Eigen::Triplet<bcg_scalar_t>>;
    using iterator_t = container_t::iterator;
    using const_iterator_t = container_t::const_iterator;

    container_t mapping;
    bcg_scalar_t min_dist, mean_dist, max_dist;
    size_t target_id;

    correspondences() = default;

    correspondences(size_t target_id) : target_id(target_id) {}

    iterator_t begin() { return mapping.begin(); }

    iterator_t end() { return mapping.end(); }

    const_iterator_t begin() const { return mapping.begin(); }

    const_iterator_t end() const { return mapping.end(); }

    void clear() { mapping.clear(); }

    void add_correspondence(size_t i, size_t j, bcg_scalar_t weight) {
        mapping.emplace_back(i, j, weight);
    }

    SparseMatrix<bcg_scalar_t> sparse_matrix(size_t M, size_t N) const {
        SparseMatrix<bcg_scalar_t> matrix(M, N);
        matrix.setFromTriplets(begin(), end());
        return matrix;
    }

    MatrixS<-1, -1> dense_matrix(size_t M, size_t N) const {
        MatrixS<-1, -1> matrix(MatrixS<-1, -1>::Zero(M, N));
        for (const auto &item : *this) {
            matrix(item.row(), item.col()) = item.value();
        }
        return matrix;
    }
};

struct entity_correspondences {
    std::unordered_map<size_t, correspondences> maps;
};

}

#endif //BCG_GRAPHICS_BCG_CORRESPONDENCES_H
