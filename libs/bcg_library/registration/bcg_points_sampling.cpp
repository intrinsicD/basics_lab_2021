//
// Created by alex on 25.05.21.
//

#include <algorithm>
#include <random>
#include "bcg_points_sampling.h"

namespace bcg{

points_sampling::points_sampling(vertex_container *vertices) : vertices(vertices), sampled_indices(){

}

void points_sampling::clear(){
    sampled_indices.clear();
}

MatrixS<-1, -1> points_sampling::embedding(const MatrixS<-1, -1> &data) const{
    MatrixS<-1, -1> E(sampled_indices.size(), data.cols());
    for(size_t i = 0; i < sampled_indices.size(); ++i){
        E.row(i) = data.row(sampled_indices[i]);
    }
    return E;
}

//----------------------------------------------------------------------------------------------------------------------

points_sampling_identity::points_sampling_identity(vertex_container *vertices) : points_sampling(vertices) {

}

void points_sampling_identity::sample() {
    clear();
    sampled_indices.resize(vertices->size());
    for(size_t i = 0; i < vertices->size(); ++i){
        sampled_indices[i] = i;
    }
}

//----------------------------------------------------------------------------------------------------------------------

points_sampling_random::points_sampling_random(vertex_container *vertices, size_t num_samples) : points_sampling_identity(vertices), num_samples(num_samples){

}

void points_sampling_random::sample() {
    sample(num_samples);
}

void points_sampling_random::sample(size_t num_samples_) {
    points_sampling_identity::sample();

    auto rd = std::random_device {};
    auto rng = std::default_random_engine { rd() };
    std::shuffle(std::begin(sampled_indices), std::end(sampled_indices), rng);
    sampled_indices.resize(std::min(num_samples_, sampled_indices.size()));
    num_samples = num_samples_;
}

}