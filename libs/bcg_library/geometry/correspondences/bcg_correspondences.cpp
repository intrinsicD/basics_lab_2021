//
// Created by alex on 08.12.20.
//

#include "bcg_correspondences.h"
#include "entt/entt.hpp"

namespace bcg {

correspondences::correspondences() : mapping(), stats(), target_id(BCG_INVALID_ID){}

correspondences::correspondences(size_t target_id) : target_id(target_id) {}

correspondences::iterator_t correspondences::begin() { return mapping.begin(); }

correspondences::iterator_t correspondences::end() { return mapping.end(); }

correspondences::const_iterator_t correspondences::begin() const { return mapping.begin(); }

correspondences::const_iterator_t correspondences::end() const { return mapping.end(); }

void correspondences::clear() {
    mapping.clear();
    stats.clear();
}

void correspondences::add_correspondence(size_t i, size_t j, bcg_scalar_t weight) {
    mapping.emplace_back(i, j, weight);
    stats.push(weight);
}

std::vector<float> correspondences::weights() const {
    std::vector<float> values(mapping.size());
    for (size_t i = 0; i < mapping.size(); ++i) {
        values[i] = mapping[i].value();
    }
    std::sort(values.begin(), values.end());
    return values;
}

SparseMatrix<bcg_scalar_t> correspondences::sparse_matrix(size_t M, size_t N) const {
    SparseMatrix<bcg_scalar_t> matrix(M, N);
    matrix.setFromTriplets(begin(), end());
    return matrix;
}

MatrixS<-1, -1> correspondences::dense_matrix(size_t M, size_t N) const {
    MatrixS<-1, -1> matrix(MatrixS<-1, -1>::Zero(M, N));
    for (const auto &item : *this) {
        matrix(item.row(), item.col()) = item.value();
    }
    return matrix;
}

MatrixS<-1, 3> correspondences::get_source_points(property<VectorS<3>, 3> source_positions) const {
    MatrixS<-1, 3> P(MatrixS<-1, 3>::Zero(mapping.size(), 3));
    for (const auto &item : mapping) {
        P.row(item.row()) = source_positions[item.row()];
    }
    return P;
}

MatrixS<-1, 3> correspondences::get_target_points(property<VectorS<3>, 3> target_positions) const {
    MatrixS<-1, 3> P(MatrixS<-1, 3>::Zero(mapping.size(), 3));
    for (const auto &item : mapping) {
        P.row(item.row()) = target_positions[item.col()];
    }
    return P;
}

MatrixS<-1, 3> correspondences::get_target_normals(property<VectorS<3>, 3> target_normals) const {
    MatrixS<-1, 3> P(MatrixS<-1, 3>::Zero(mapping.size(), 3));
    for (const auto &item : mapping) {
        P.row(item.row()) = target_normals[item.col()];
    }
    return P;
}

}