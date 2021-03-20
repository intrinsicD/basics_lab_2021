//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_ALIGNED_BOX_H
#define BCG_GRAPHICS_BCG_ALIGNED_BOX_H

#include <vector>
#include <iostream>
#include "math/bcg_linalg.h"
#include "utils/bcg_bits.h"

namespace bcg {

template<int N>
struct aligned_box {
    VectorS<N> min, max;

    aligned_box() : aligned_box(VectorS<N>::Constant(scalar_max), VectorS<N>::Constant(scalar_min)) {}

    aligned_box(const VectorS<N> &min, const VectorS<N> &max) : min(min), max(max) {}

    explicit aligned_box(const std::vector<VectorS<N>> &points) : aligned_box() {
        for (const auto &p : points) {
            grow(p);
        }
    }

    inline static aligned_box unit() {
        return aligned_box(-ones<N>, ones<N>);
    }

    inline void set_centered_form(const VectorS<N> &center, const VectorS<N> &half_extent) {
        min = center - half_extent;
        max = center + half_extent;
    }

    inline void get_centered_form(VectorS<N> &center, VectorS<N> &half_extent) const {
        center = this->center();
        half_extent = halfextent();
    }

    inline VectorS<N> center() const {
        return (max + min) / 2;
    }

    inline VectorS<N> halfextent() const {
        return diagonal() / 2;
    }

    inline void grow(const VectorS<N> &p) {
        min = min.cwiseMin(p);
        max = max.cwiseMax(p);
    }

    inline VectorS<N> diagonal() const {
        return max - min;
    }

    inline aligned_box merge(const aligned_box &other) const {
        return aligned_box(min.min(other.min), max.max(other.max));
    }

    inline void make_cubic() {
        VectorS<N> center, halfextent;
        get_centered_form(center, halfextent);
        set_centered_form(center, VectorS<N>::Constant(halfextent.maxCoeff()));
    }

    inline bool contains(const VectorS<N> &point) const {
        for (size_t i = 0; i < N; ++i) {
            if (point[i] < min[i] || point[i] > max[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator==(const aligned_box &box) const {
        return min == box.min && max == box.max;
    }

    bool operator!=(const aligned_box &box) const {
        return !operator==(box);
    }

    bool operator<(const aligned_box &box) const {
        if (min < box.min) {
            return true;
        }

        if (min > box.min) {
            return false;
        }

        return max < box.max;
    }

    bool operator<=(const aligned_box &box) const {
        return !box.operator<(*this);
    }

    bool operator>(const aligned_box &box) const {
        return box.operator<(*this);
    }

    bool operator>=(const aligned_box &box) const {
        return !operator<(box);
    }
};

template<int N>
inline std::ostream &operator<<(std::ostream &stream, const aligned_box<N> &alignedBox) {
    stream << "min: " << alignedBox.min << "\n";
    stream << "max: " << alignedBox.max << "\n";
    VectorS<N> center, halfextent;
    alignedBox.get_centered_form(center, halfextent);
    stream << "center: " << center << "\n";
    stream << "halfextent: " << halfextent << "\n";
    return stream;
}

/*               e11
 *                v
 *         v6-------------v7
 *        /|             /|
 *   e6 >/ |< e9    e7 >/ |
 *      /  |   e5      /  |< e10
 *     /   |   v      /   |
 *    v2-------------v3   |
 *    |    v4--------|----v5
 *e1 >|   /     ^    |   /
 *    |  /     e8    |  / < e4
 *    | /< e2    e3 >| /
 *    |/             |/
 *    v0-------------v1
 *           ^
 *           e0
 * */

template<int N>
std::vector<VectorS<N>> get_vetices(const aligned_box<N> &alignedBox) {
    VectorS<N> center, halfextent;
    alignedBox.get_centered_form(center, halfextent);

    std::vector<VectorS<N>> vertices;

    size_t num_vertices = BIT(N);
    for (unsigned int i = 0; i < num_vertices; ++i) {
        vertices.push_back(center);
        for (unsigned int d = 0, mask = 1; d < N; ++d, mask <<= (unsigned int) 1) {
            bcg_scalar_t sign = (i & mask ? (bcg_scalar_t) 1 : (bcg_scalar_t) -1);
            vertices[i][d] += sign * halfextent[d];
        }
    }
    assert(vertices.size() == num_vertices);
    return vertices;
}

template<int N>
std::vector<VectorI<2>> get_edges(const aligned_box<N> &, size_t offset = 0) {
    std::vector<VectorI<2>> edges;
    size_t n = BIT(N);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i; j < n; ++j) {
            if (HAMMING(i, j) == 1) {
                edges.template emplace_back(static_cast<bcg_index_t>(offset + i), static_cast<bcg_index_t>(offset + j));
            }
        }
    }
    BCG_MAYBE_UNUSED size_t num_eges = (BIT(N - 1)) * N;
    assert(edges.size() == num_eges);
    return edges;
}

template<int N>
std::vector<VectorI<3>> get_triangles(const aligned_box<N> &, size_t offset = 0) {
    std::vector<VectorI<3>> tris;
    VectorI<3> o = VectorI<3>::Constant(bcg_index_t(offset));

    tris.emplace_back(VectorI<3>({1, 0, 3}) + o);
    tris.emplace_back(VectorI<3>({3, 0, 2}) + o);
    tris.emplace_back(VectorI<3>({3, 2, 7}) + o);
    tris.emplace_back(VectorI<3>({7, 2, 6}) + o);
    tris.emplace_back(VectorI<3>({0, 4, 2}) + o);
    tris.emplace_back(VectorI<3>({2, 4, 6}) + o);
    tris.emplace_back(VectorI<3>({5, 1, 7}) + o);
    tris.emplace_back(VectorI<3>({7, 1, 3}) + o);
    tris.emplace_back(VectorI<3>({5, 4, 1}) + o);
    tris.emplace_back(VectorI<3>({1, 4, 0}) + o);
    tris.emplace_back(VectorI<3>({7, 6, 5}) + o);
    tris.emplace_back(VectorI<3>({5, 6, 4}) + o);

    return tris;
}

template<int N>
std::vector<VectorI<4>> get_faces(const aligned_box<N> &, size_t offset = 0) {
    std::vector<VectorI<4>> faces;
    VectorI<4> o = VectorI<4>::Constant(bcg_index_t(offset));

    faces.emplace_back(VectorI<4>(0, 1, 3, 2) + o);
    faces.emplace_back(VectorI<4>(1, 5, 7, 3) + o);
    faces.emplace_back(VectorI<4>(2, 3, 7, 6) + o);
    faces.emplace_back(VectorI<4>(4, 5, 1, 0) + o);
    faces.emplace_back(VectorI<4>(4, 0, 2, 6) + o);
    faces.emplace_back(VectorI<4>(6, 7, 5, 4) + o);

    return faces;
}

using aligned_box2 = aligned_box<2>;

using aligned_box3 = aligned_box<3>;

}
#endif //BCG_GRAPHICS_BCG_ALIGNED_BOX_H
