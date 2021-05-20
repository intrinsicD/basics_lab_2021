//
// Created by alex on 20.05.21.
//

#include "bcg_filter_correspondence_mutual.h"
#include "math/matrix/bcg_matrix_max.h"

namespace bcg{

filter_correspondence_mutual::filter_correspondence_mutual() : filter() {

}

void filter_correspondence_mutual::apply(const correspondences &corrs) {
    clear();

    std::vector<Eigen::Triplet<int>> triplets;
    for (bcg_index_t i = 0; i < corrs.size(); ++i) {
        const auto &item = corrs.mapping[i];
        triplets.emplace_back(i, item.row() * item.col(), 1);
    }
    SparseMatrix<bcg_index_t> matrix(corrs.size(), corrs.max_source_index * corrs.max_target_index);
    matrix.setFromTriplets(triplets.begin(), triplets.end());

    VectorI<-1> indices_to;
    VectorI<-1> counts = mat_max(matrix.toDense(), 1, indices_to);

    for (bcg_index_t i = 0; i < indices_to.size(); ++i) {
        if (counts[i] > 1) {
            filtered.emplace_back(i);
        }
    }
}

bool filter_correspondence_mutual::apply_element(const Triplet &corr1, const Triplet &corr2, bcg_index_t i){
    if (corr1.row() == corr2.col() && corr1.col() == corr2.row()) {
        filtered.emplace_back(i);
        return true;
    }
    return false;
}

correspondences filter_correspondence_mutual::get_filtered_correspondences(const correspondences &corrs) const {
    correspondences F(corrs.target_id);
    for (unsigned int i : filtered) {
        const auto &item = corrs.mapping[i];
        F.add_correspondence(item.row(), item.col(), item.value());
    }
    return F;
}

}