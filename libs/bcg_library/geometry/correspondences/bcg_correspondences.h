//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_CORRESPONDENCES_H
#define BCG_GRAPHICS_BCG_CORRESPONDENCES_H

#include "point_cloud/bcg_point_cloud.h"
#include "math/sparse_matrix/bcg_sparse_matrix.h"
#include "math/statistics/bcg_statistics_running.h"

namespace bcg {

struct correspondences {
    using container_t = std::vector<Eigen::Triplet<bcg_scalar_t>>;
    using iterator_t = container_t::iterator;
    using const_iterator_t = container_t::const_iterator;

    container_t mapping;
    running_stats stats;
    size_t target_id;
    size_t max_source_index, max_target_index;

    correspondences();

    explicit correspondences(size_t target_id);

    iterator_t begin();

    iterator_t end();

    [[nodiscard]] const_iterator_t begin() const;

    [[nodiscard]] const_iterator_t end() const;

    void clear();

    [[nodiscard]] size_t size() const;

    void append(const correspondences &other);

    void merge_unique(const correspondences &other);

    void add_correspondence(size_t i, size_t j, bcg_scalar_t weight);

    std::vector<bcg_scalar_t> weights() const;

    std::vector<bcg_index_t> source_indices() const;

    std::vector<bcg_index_t> target_indices() const;

    SparseMatrix<bcg_scalar_t> sparse_matrix(size_t M, size_t N) const;

    MatrixS<-1, -1> dense_matrix(size_t M, size_t N) const;

    MatrixS<-1, 3> get_source(property<VectorS<3>, 3> property) const;

    MatrixS<-1, 3> get_target(property<VectorS<3>, 3> property) const;
};

struct entity_correspondences {
    std::unordered_map<size_t, correspondences> maps;
};

}

#endif //BCG_GRAPHICS_BCG_CORRESPONDENCES_H
