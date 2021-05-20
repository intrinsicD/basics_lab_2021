//
// Created by alex on 19.05.21.
//

#ifndef BCG_GRAPHICS_BCG_FILTER_H
#define BCG_GRAPHICS_BCG_FILTER_H

#include "math/bcg_linalg.h"

namespace bcg {

struct filter {
    using container_t = std::vector<bcg_index_t>;
    using iterator_t = container_t::iterator;
    using const_iterator_t = container_t::const_iterator;

    std::vector<bcg_index_t> filtered;

    filter() = default;

    virtual ~filter() = default;

    iterator_t begin();

    iterator_t end();

    [[nodiscard]] const_iterator_t begin() const;

    [[nodiscard]] const_iterator_t end() const;

    void clear();
};


template<typename Derived>
struct filter_vector : public filter {
    using Scalar = typename Derived::Scalar;
    using VectorRef = Ref<Derived>;
    using Comparator = std::function<bool(Scalar element, Scalar threshold)>;

    filter_vector() = default;

    explicit filter_vector(const Comparator &comparator)
            : filter(), comparator(comparator) {

    }

    ~filter_vector() override = default;

    std::function<bool(Scalar element, Scalar threshold)> comparator;

    void apply(const VectorRef &data, Scalar threshold) {
        clear();
        for (bcg_index_t i = 0; i < data.size(); ++i) {
            apply_element(data(i), i, threshold);
        }
    }


    bool apply_element(const Scalar value, bcg_index_t i, Scalar threshold) {
        if (comparator(value, threshold)) {
            filtered.emplace_back(i);
            return true;
        }
        return false;
    }

    Derived get_filtered_embedding(const VectorRef &data) const {
        Derived F(filtered.size(), data.cols());
        for (size_t i = 0; i < filtered.size(); ++i) {
            F.row(i) = data(filtered[i]);
        }
        return F;
    }
};

template<typename Derived>
struct filter_matrix : public filter {
    using Scalar = typename Derived::Scalar;
    using VectorRef = Ref<Derived>;
    using MatrixRef = Ref<Derived>;
    using Comparator = std::function<bool(const VectorRef &row1, const VectorRef &row2, Scalar threshold)>;

    filter_matrix() = default;

    explicit filter_matrix(const Comparator &comparator) : filter(), comparator(comparator) {

    }

    ~filter_matrix() override = default;

    std::function<bool(const VectorRef &row1, const VectorRef &row2, Scalar threshold)> comparator;

    void apply(const MatrixRef &source, const MatrixRef &target, Scalar threshold) {
        clear();
        for (bcg_index_t i = 0; i < source.rows(); ++i) {
            apply_element(source.row(i), target.row(i), i, threshold);
        }
    }

    bool apply_element(const VectorRef &row1, const VectorRef &row2, bcg_index_t i, Scalar threshold) {
        if (comparator(row1, row2, threshold)) {
            filtered.emplace_back(i);
            return true;
        }
        return false;
    }

    Derived get_filtered_embedding(const MatrixRef &data) const {
        Derived F(filtered.size(), data.cols());
        for (size_t i = 0; i < filtered.size(); ++i) {
            F.row(i) = data.row(filtered[i]);
        }
        return F;
    }
};

}

#endif //BCG_GRAPHICS_BCG_FILTER_H
