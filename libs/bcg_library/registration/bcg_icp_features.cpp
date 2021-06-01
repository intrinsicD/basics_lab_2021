//
// Created by alex on 24.05.21.
//

#include <algorithm>
#include "bcg_icp_features.h"
#include "bcg_property_map_eigen.h"
#include "math/matrix/bcg_matrix_horizontal_stack.h"

namespace bcg {

Feature::Feature(std::string name) : property_name(std::move(name)){

}

bool Feature::operator==(const std::string &name) const {
    return property_name == name;
}

bool Feature::operator!=(const std::string &name) const {
    return property_name != name;
}

bool Feature::operator==(const Feature &other) const {
    return property_name == other.property_name;
}

bool Feature::operator!=(const Feature &other) const {
    return property_name != other.property_name;
}

MatrixS<-1, -1> Feature::build(const vertex_container &container) const {
    if (container.has(property_name)) {
        return MapConst<bcg_scalar_t>(container.get_base_ptr(property_name));
    }
    return MatrixS<-1, -1>();
}

//----------------------------------------------------------------------------------------------------------------------

FeatureBatch::FeatureBatch(std::string batch_name) : batch_name(std::move(batch_name)) {

}

FeatureBatch::FeatureBatch(std::string batch_name, std::unordered_map<std::string, Feature> batch) : batch_name(
        std::move(batch_name)),
                                                                                                     batch(std::move(
                                                                                                             batch)) {

}

bool FeatureBatch::add_property(const std::string &name) {
    if (!has_property(name)) {
        batch[name] = Feature(name);
        return true;
    }
    return false;
}

bool FeatureBatch::has_property(const std::string &name) const {
    return batch.find(name) != batch.end();
}

bool FeatureBatch::remove_property(const std::string &name) {
    auto iter = batch.find(name);
    if (iter != batch.end()) {
        batch.erase(iter);
        return true;
    }
    return false;
}

bool FeatureBatch::operator==(const FeatureBatch &other) const {
    if (batch.size() != other.batch.size()) return false;

    for (const auto &item : batch) {
        if (other.batch.find(item.first) == other.batch.end()) return false;
    }
    return true;
}

bool FeatureBatch::operator!=(const FeatureBatch &other) const {
    if (batch.size() != other.batch.size()) return true;

    for (const auto &item : batch) {
        if (other.batch.find(item.first) == other.batch.end()) return true;
    }
    return false;
}

void FeatureBatch::build(const vertex_container &container) {
    std::vector<MatrixS<-1, -1>, Eigen::aligned_allocator<MatrixS<-1, -1>>> embedding;
    for (const auto &item : batch) {
        embedding.push_back(item.second.build(container));
    }
    batch_embedding = horizontal_stack(embedding);
}

//----------------------------------------------------------------------------------------------------------------------

Features::Features(std::unordered_map<std::string, FeatureBatch> batches) : batches(std::move(batches)) {

}

bool Features::add_property(const std::string &batch_name, const std::string &property_name) {
    if (!has_batch(batch_name)) {
        batches[batch_name] = FeatureBatch(batch_name);
    }
    auto &feature_batch = batches[batch_name];
    auto iter = feature_batch.batch.find(property_name);
    if (iter == feature_batch.batch.end()) {
        feature_batch.add_property(property_name);
        return true;
    }
    return false;
}

bool Features::has_property(const std::string &batch_name, const std::string &property_name) const {
    auto iter = batches.find(batch_name);
    if (iter == batches.end()) return false;
    return iter->second.has_property(property_name);
}

bool Features::remove_property(const std::string &batch_name, const std::string &property_name) {
    auto iter = batches.find(batch_name);
    if (iter == batches.end()) return false;
    bool success = iter->second.remove_property(property_name);
    if (iter->second.batch.empty()) {
        batches.erase(iter);
        success |= true;
    }
    return success;
}

bool Features::has_batch(const std::string &batch_name) const {
    return batches.find(batch_name) != batches.end();
}

void Features::build(const vertex_container &container) {
    std::vector<MatrixS<-1, -1>> embeddings;
    for (auto &item : batches) {
        item.second.build(container);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}