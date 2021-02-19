//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_CORRESPONDENCES_H
#define BCG_GRAPHICS_BCG_CORRESPONDENCES_H

#include <unordered_map>
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

    correspondences();

    explicit correspondences(size_t target_id);

    iterator_t begin();

    iterator_t end();

    const_iterator_t begin() const;

    const_iterator_t end() const;

    void clear();

    void add_correspondence(size_t i, size_t j, bcg_scalar_t weight);

    std::vector<bcg_scalar_t> weights() const;

    SparseMatrix<bcg_scalar_t> sparse_matrix(size_t M, size_t N) const;

    MatrixS<-1, -1> dense_matrix(size_t M, size_t N) const;

    MatrixS<-1, 3> get_source_points(property<VectorS<3>, 3> source_positions) const;

    MatrixS<-1, 3> get_target_points(property<VectorS<3>, 3> target_positions) const;

    MatrixS<-1, 3> get_target_normals(property<VectorS<3>, 3> target_normals) const;
};

struct entity_correspondences {
    std::unordered_map<size_t, correspondences> maps;
};

}

#endif //BCG_GRAPHICS_BCG_CORRESPONDENCES_H
