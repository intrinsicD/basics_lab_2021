//
// Created by alex on 31.05.21.
//

#ifndef BCG_GRAPHICS_BCG_FILTER_H
#define BCG_GRAPHICS_BCG_FILTER_H

#include <string>
#include <functional>
#include <utility>
#include <unordered_map>
#include "math/bcg_linalg.h"

namespace bcg {

struct filter {
    filter() = default;

    filter(std::string name,
           std::function<bool(bcg_scalar_t, bcg_scalar_t)> comparator,
           std::function<bcg_scalar_t(size_t, size_t)> metric, bcg_scalar_t threshold) : name(std::move(name)),
                                                                                         comparator(
                                                                                                 std::move(comparator)),
                                                                                         metric(std::move(metric)),
                                                                                         threshold(threshold) {

    }

    std::string name;
    std::function<bool(bcg_scalar_t, bcg_scalar_t)> comparator;
    std::function<bcg_scalar_t(size_t, size_t)> metric;
    bcg_scalar_t threshold;

    [[nodiscard]] bool compare(size_t i, size_t j) const {
        return comparator(metric(i, j), threshold);
    }

    [[nodiscard]] bcg_scalar_t distance(size_t i, size_t j) const {
        return metric(i, j);
    }
};

struct filter_chain {
    using container_t = std::unordered_map<std::string, filter>;
    using iterator_t = container_t::iterator;
    using const_iterator_t = container_t::const_iterator;

    std::string name;
    container_t chain;

    explicit filter_chain(std::string name) : name(std::move(name)) {}

    iterator_t begin() {
        return chain.begin();
    }

    iterator_t end() {
        return chain.end();
    }

    const_iterator_t begin() const {
        return chain.begin();
    }

    const_iterator_t end() const {
        return chain.end();
    }

    void add_filter(const filter &f) {
        chain[f.name] = f;
    }

    void remove_filter(const std::string &filter_name) {
        auto iter = chain.find(filter_name);
        if (iter != chain.end()) {
            chain.erase(iter);
        }
    }

    std::vector<std::string> filter_names() const {
        std::vector<std::string> names;
        for (const auto &item : chain) {
            names.push_back(item.first);
        }
        return names;
    }

    [[nodiscard]] bool compare(size_t i, size_t j) const {
        return std::all_of(begin(), end(), [i, j](const filter &f) {
            return f.compare(i, j);
        });
    }
};

struct filter_pipeline {
    using container_t = std::unordered_map<std::string, filter_chain>;
    using iterator_t = container_t::iterator;
    using const_iterator_t = container_t::const_iterator;

    std::string name;
    container_t chains;

    explicit filter_pipeline(std::string name) : name(std::move(name)) {}

    iterator_t begin() {
        return chains.begin();
    }

    iterator_t end() {
        return chains.end();
    }

    const_iterator_t begin() const {
        return chains.begin();
    }

    const_iterator_t end() const {
        return chains.end();
    }

    std::vector<std::string> chain_names() const {
        std::vector<std::string> names;
        for (const auto &item : chains) {
            names.push_back(item.first);
        }
        return names;
    }

    void add_chain(const std::string &chain_name) {
        chains[chain_name] = filter_chain(chain_name);
    }

    void remove_chain(const std::string &chain_name) {
        auto iter = chains.find(chain_name);
        if (iter != end()) {
            chains.erase(iter);
        }
    }

    void add_filter(const std::string &chain_name, const filter &f) {
        chains[chain_name].add_filter(f);
    }

    void remove_filter(const std::string &chain_name, const std::string &filter_name) {
        auto iter = chains.find(chain_name);
        if (iter != end()) {
            iter->second.remove_filter(filter_name);
        }
    }

    [[nodiscard]] bool compare(size_t i, size_t j) const {
        return std::any_of(begin(), end(), [i, j](const filter_chain &fc) {
            return fc.compare(i, j);
        });
    }
};

struct filter_factory_normal_angle {
    static filter make_normal_filter_less(vertex_container &source,
                                          vertex_container &target,
                                          bcg_scalar_t threshold,
                                          const std::string &source_name,
                                          const std::string &target_name) {
        auto source_property = source.get_or_add<VectorS<3>, 3>(source_name);
        auto target_property = target.get_or_add<VectorS<3>, 3>(target_name);

        std::function<bcg_scalar_t(size_t, size_t)> metric = [source_property, target_property](size_t i, size_t j) {
            return source_property[i].transpose() * target_property[j];
        };

        std::function<bool(bcg_scalar_t, bcg_scalar_t)> comparator = [](bcg_scalar_t value, bcg_scalar_t threshold) {
            return value < threshold;
        };
        return filter("normal_angle_less", comparator, metric, threshold);
    }

    static filter make_normal_filter_less_eq(vertex_container &source,
                                             vertex_container &target,
                                             bcg_scalar_t threshold,
                                             const std::string &source_name,
                                             const std::string &target_name) {
        auto source_property = source.get_or_add<VectorS<3>, 3>(source_name);
        auto target_property = target.get_or_add<VectorS<3>, 3>(target_name);

        std::function<bcg_scalar_t(size_t, size_t)> metric = [source_property, target_property](size_t i, size_t j) {
            return source_property[i].transpose() * target_property[j];
        };

        std::function<bool(bcg_scalar_t, bcg_scalar_t)> comparator = [](bcg_scalar_t value, bcg_scalar_t threshold) {
            return value <= threshold;
        };
        return filter("normal_angle_less_eq", comparator, metric, threshold);
    }

    static filter make_normal_filter_greater_eq(vertex_container &source,
                                                vertex_container &target,
                                                bcg_scalar_t threshold,
                                                const std::string &source_name,
                                                const std::string &target_name) {
        auto source_property = source.get_or_add<VectorS<3>, 3>(source_name);
        auto target_property = target.get_or_add<VectorS<3>, 3>(target_name);

        std::function<bcg_scalar_t(size_t, size_t)> metric = [source_property, target_property](size_t i, size_t j) {
            return source_property[i].transpose() * target_property[j];
        };

        std::function<bool(bcg_scalar_t, bcg_scalar_t)> comparator = [](bcg_scalar_t value, bcg_scalar_t threshold) {
            return value >= threshold;
        };
        return filter("normal_angle_greater_eq", comparator, metric, threshold);
    }

    static filter make_normal_filter_greater(vertex_container &source,
                                             vertex_container &target,
                                             bcg_scalar_t threshold,
                                             const std::string &source_name,
                                             const std::string &target_name) {
        auto source_property = source.get_or_add<VectorS<3>, 3>(source_name);
        auto target_property = target.get_or_add<VectorS<3>, 3>(target_name);

        std::function<bcg_scalar_t(size_t, size_t)> metric = [source_property, target_property](size_t i, size_t j) {
            return source_property[i].transpose() * target_property[j];
        };

        std::function<bool(bcg_scalar_t, bcg_scalar_t)> comparator = [](bcg_scalar_t value, bcg_scalar_t threshold) {
            return value > threshold;
        };
        return filter("normal_angle_greater", comparator, metric, threshold);
    }
};


struct filter_factory_distance {
    static filter make_distance_filter_less(vertex_container &source,
                                          vertex_container &target,
                                          bcg_scalar_t threshold,
                                          const std::string &source_name,
                                          const std::string &target_name) {
        auto source_property = source.get_or_add<VectorS<3>, 3>(source_name);
        auto target_property = target.get_or_add<VectorS<3>, 3>(target_name);

        std::function<bcg_scalar_t(size_t, size_t)> metric = [source_property, target_property](size_t i, size_t j) {
            return (source_property[i] - target_property[j]).norm();
        };

        std::function<bool(bcg_scalar_t, bcg_scalar_t)> comparator = [](bcg_scalar_t value, bcg_scalar_t threshold) {
            return value < threshold;
        };
        return filter("distance_less", comparator, metric, threshold);
    }

    static filter make_normal_filter_less_eq(vertex_container &source,
                                             vertex_container &target,
                                             bcg_scalar_t threshold,
                                             const std::string &source_name,
                                             const std::string &target_name) {
        auto source_property = source.get_or_add<VectorS<3>, 3>(source_name);
        auto target_property = target.get_or_add<VectorS<3>, 3>(target_name);

        std::function<bcg_scalar_t(size_t, size_t)> metric = [source_property, target_property](size_t i, size_t j) {
            return (source_property[i] - target_property[j]).norm();
        };

        std::function<bool(bcg_scalar_t, bcg_scalar_t)> comparator = [](bcg_scalar_t value, bcg_scalar_t threshold) {
            return value <= threshold;
        };
        return filter("distance_less_eq", comparator, metric, threshold);
    }

    static filter make_normal_filter_greater_eq(vertex_container &source,
                                                vertex_container &target,
                                                bcg_scalar_t threshold,
                                                const std::string &source_name,
                                                const std::string &target_name) {
        auto source_property = source.get_or_add<VectorS<3>, 3>(source_name);
        auto target_property = target.get_or_add<VectorS<3>, 3>(target_name);

        std::function<bcg_scalar_t(size_t, size_t)> metric = [source_property, target_property](size_t i, size_t j) {
            return (source_property[i] - target_property[j]).norm();
        };

        std::function<bool(bcg_scalar_t, bcg_scalar_t)> comparator = [](bcg_scalar_t value, bcg_scalar_t threshold) {
            return value >= threshold;
        };
        return filter("distance_greater_eq", comparator, metric, threshold);
    }

    static filter make_normal_filter_greater(vertex_container &source,
                                             vertex_container &target,
                                             bcg_scalar_t threshold,
                                             const std::string &source_name,
                                             const std::string &target_name) {
        auto source_property = source.get_or_add<VectorS<3>, 3>(source_name);
        auto target_property = target.get_or_add<VectorS<3>, 3>(target_name);

        std::function<bcg_scalar_t(size_t, size_t)> metric = [source_property, target_property](size_t i, size_t j) {
            return (source_property[i] - target_property[j]).norm();
        };

        std::function<bool(bcg_scalar_t, bcg_scalar_t)> comparator = [](bcg_scalar_t value, bcg_scalar_t threshold) {
            return value > threshold;
        };
        return filter("distance_greater", comparator, metric, threshold);
    }
};

}

#endif //BCG_GRAPHICS_BCG_FILTER_H
