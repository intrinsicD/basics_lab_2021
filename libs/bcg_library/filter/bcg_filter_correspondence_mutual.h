//
// Created by alex on 20.05.21.
//

#ifndef BCG_GRAPHICS_BCG_FILTER_CORRESPONDENCE_MUTUAL_H
#define BCG_GRAPHICS_BCG_FILTER_CORRESPONDENCE_MUTUAL_H

#include "bcg_filter.h"
#include "correspondences/bcg_correspondences.h"
#include "math/sparse_matrix/bcg_sparse_matrix.h"

namespace bcg {

struct filter_correspondence_mutual : public filter {
    using Triplet = Eigen::Triplet<bcg_scalar_t>;

    filter_correspondence_mutual();

    ~filter_correspondence_mutual() override = default;

    void apply(const correspondences &corrs);

    bool apply_element(const Triplet &corr1, const Triplet &corr2, bcg_index_t i);

    correspondences get_filtered_correspondences(const correspondences &corrs) const;
};

}

#endif //BCG_GRAPHICS_BCG_FILTER_CORRESPONDENCE_MUTUAL_H
