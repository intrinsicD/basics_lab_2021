//
// Created by alex on 08.12.20.
//

#include "bcg_correspondences.h"
#include <unordered_map>

namespace bcg {

correspondences::correspondences() : correspondences(BCG_INVALID_ID) {}

correspondences::correspondences(size_t target_id)
        : mapping(), stats(), target_id(target_id), max_source_index(0), max_target_index(0) {}

correspondences::iterator_t correspondences::begin() { return mapping.begin(); }

correspondences::iterator_t correspondences::end() { return mapping.end(); }

correspondences::const_iterator_t correspondences::begin() const { return mapping.begin(); }

correspondences::const_iterator_t correspondences::end() const { return mapping.end(); }

void correspondences::clear() {
    mapping.clear();
    stats.clear();
}

size_t correspondences::size() const {
    return mapping.size();
}

void correspondences::add_correspondence(size_t i, size_t j, bcg_scalar_t weight) {
    mapping.emplace_back(i, j, weight);
    stats.push(weight);
    if(i > max_source_index){
        max_source_index = i;
    }
    if(j > max_target_index){
        max_target_index = j;
    }
}

void correspondences::append(const correspondences &other){
    mapping.resize( mapping.size() + other.size() ); // preallocate memory
    mapping.insert( mapping.end(), other.begin(), other.end() );
    max_source_index = std::max(max_source_index, other.max_source_index);
    max_target_index = std::max(max_target_index, other.max_target_index);
    stats += other.stats;
}

void correspondences::merge_unique(const correspondences &other){
    std::unordered_map<bcg_index_t , Eigen::Triplet<bcg_scalar_t>> unique;
    for(const auto &item: mapping){
        unique[item.row()] = item;
    }
    for(const auto &item: other){
        if(unique.find(item.row()) != unique.end()){
            unique[item.row()] = item;
        }
    }

    clear();
    for(const auto &item : unique){
        add_correspondence(item.second.row(), item.second.col(), item.second.value());
    }
}

std::vector<bcg_scalar_t> correspondences::weights() const {
    std::vector<bcg_scalar_t> values(mapping.size());
    for (size_t i = 0; i < mapping.size(); ++i) {
        values[i] = mapping[i].value();
    }
    return values;
}

std::vector<bcg_index_t> correspondences::source_indices() const{
    std::vector<bcg_index_t> values(mapping.size());
    for (size_t i = 0; i < mapping.size(); ++i) {
        values[i] = mapping[i].row();
    }
    return values;
}

std::vector<bcg_index_t> correspondences::target_indices() const{
    std::vector<bcg_index_t> values(mapping.size());
    for (size_t i = 0; i < mapping.size(); ++i) {
        values[i] = mapping[i].col();
    }
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

MatrixS<-1, 3> correspondences::get_source(property<VectorS<3>, 3> property) const {
    MatrixS<-1, 3> P(MatrixS<-1, 3>::Zero(static_cast<long>(mapping.size()), 3));
    for (long i = 0; i < static_cast<long>(mapping.size()); ++i) {
        P.row(i) = property[mapping[i].row()];
    }
    return P;
}

MatrixS<-1, 3> correspondences::get_target(property<VectorS<3>, 3> property) const {
    MatrixS<-1, 3> P(MatrixS<-1, 3>::Zero(static_cast<long>(mapping.size()), 3));
    for (long i = 0; i < static_cast<long>(mapping.size()); ++i) {
        P.row(i) = property[mapping[i].col()];
    }
    return P;
}

}