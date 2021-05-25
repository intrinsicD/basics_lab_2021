//
// Created by alex on 24.05.21.
//

#ifndef BCG_GRAPHICS_BCG_ICP_FEATURES_H
#define BCG_GRAPHICS_BCG_ICP_FEATURES_H

#include <unordered_map>
#include "kdtree/bcg_kdtree.h"

namespace bcg {
struct Feature {
    Feature() = default;

    explicit Feature(std::string name);

    bool operator==(const std::string &name) const;

    bool operator!=(const std::string &name) const;

    bool operator==(const Feature &other) const;

    bool operator!=(const Feature &other) const;

    MatrixS<-1, -1> build(const vertex_container &container) const;

    std::string property_name;
};

struct FeatureBatch {
    FeatureBatch() = default;

    explicit FeatureBatch(std::string batch_name);

    FeatureBatch(std::string batch_name, std::unordered_map<std::string, Feature> batch);

    bool add_property(const std::string &name);

    bool has_property(const std::string &name) const;

    bool remove_property(const std::string &name);

    bool operator==(const FeatureBatch &other) const;

    bool operator!=(const FeatureBatch &other) const;

    void build(const vertex_container &container, bool with_index = true);

    std::string batch_name;
    std::unordered_map<std::string, Feature> batch;
    MatrixS<-1, -1> batch_embedding;
    std::unique_ptr<kdtree_matrix<bcg_scalar_t>> index;
};

struct Features {
    Features() = default;

    explicit Features(std::unordered_map<std::string, FeatureBatch> batches);

    bool add_property(const std::string &batch_name, const std::string &property_name);

    bool has_property(const std::string &batch_name, const std::string &property_name) const;

    bool remove_property(const std::string &batch_name, const std::string &property_name);

    bool has_batch(const std::string &batch_name) const;

    void build(const vertex_container &container, bool with_indices = true);

    std::unordered_map<std::string, FeatureBatch> batches;
};

}

#endif //BCG_GRAPHICS_BCG_ICP_FEATURES_H
