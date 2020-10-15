//
// Created by alex on 08.10.20.
//

#ifndef BCG_GRAPHICS_BCG_ALIGNED_BOX_H
#define BCG_GRAPHICS_BCG_ALIGNED_BOX_H

// -----------------------------------------------------------------------------
// INCLUDES
// -----------------------------------------------------------------------------

#include <vector>
#include <iostream>
#include "bcg_library/math/bcg_math_common.h"
#include "bcg_library/math/bcg_linalg.h"
#include "bcg_library/utils/bcg_bits.h"

namespace bcg {

template<int N>
struct aligned_box {
    using vecf = vec<N, float>;

    vecf min, max;

    aligned_box(const vecf &min, const vecf &max) : min(min), max(max) {}

    explicit aligned_box(const std::vector<vecf> &points) : aligned_box() {
        for (const auto &p : points) {
            grow(p);
        }
    }

    inline static aligned_box unit() {
        return aligned_box(-one < N > , one < N > );
    }

    inline void set_centered_form(const vecf &center, const vecf &half_extent) {
        min = center - half_extent;
        max = center + half_extent;
    }

    inline void get_centered_form(vecf &center, vecf &half_extent) const {
        center = this->center();
        half_extent = halfextent();
    }

    inline vecf center() const {
        return (max + min) / 2;
    }

    inline vecf halfextent() const {
        return diagonal() / 2;
    }

    inline void grow(const vecf &p) {
        min = min.cwiseMin(p);
        max = max.cwiseMax(p);
    }

    inline vecf diagonal() const {
        return max - min;
    }

    inline aligned_box merge(const aligned_box &other) const {
        return aligned_box(min.min(other.min), max.max(other.max));
    }

    inline void make_cubic() {
        vecf center, halfextent;
        get_centered_form(center, halfextent);
        set_centered_form(center, vecf::Constant(halfextent.maxCoeff()));
    }
};

template<int N>
inline std::ostream &operator<<(std::ostream &stream, const aligned_box<N> &alignedBox) {
    stream << "min: " << alignedBox.min << "\n";
    stream << "max: " << alignedBox.max << "\n";
    typename aligned_box<N>::vecf center, halfextent;
    alignedBox.get_centered_form(center, halfextent);
    stream << "center: " << center << "\n";
    stream << "halfextent: " << halfextent << "\n";
    return stream;
}

template<int N>
std::vector<typename aligned_box<N>::vecf> get_vetices(const aligned_box<N> &alignedBox) {
    typename aligned_box<N>::vecf center, halfextent;
    alignedBox.get_centered_form(center, halfextent);

    std::vector<typename aligned_box<N>::vecf> vertices;
    for (unsigned int i = 0; i < alignedBox.NumVertices(); ++i) {
        vertices.push_back(center);
        for (unsigned int d = 0, mask = 1; d < N; ++d, mask <<= (unsigned int) 1) {
            float sign = (i & mask ? (float) 1 : (float) - 1);
            vertices[i][d] += sign * halfextent[d];
        }
    }
    assert(vertices.size() == alignedBox.NumVertices());
    return vertices;
}

template<int N>
std::vector<vec2i> fet_edges(const aligned_box<N> &alignedBox, size_t offset = 0) {
    std::vector<vec2i> edges;
    size_t n = alignedBox.NumVertices();
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i; j < n; ++j) {
            if (HAMMING(i, j) == 1) {
                edges.push_back({static_cast<int>(offset + i), static_cast<int>(offset + j)});
            }
        }
    }
    assert(edges.size() == alignedBox.NumEdges());
    return edges;
}

template<int N>
std::vector<vec3i> get_triangles(const aligned_box<N> &, size_t offset = 0) {
    std::vector<vec3i> tris;
    vec3i o(offset, offset, offset);

    tris.push_back(vec3i({1, 0, 3}) + o);
    tris.push_back(vec3i({3, 0, 2}) + o);
    tris.push_back(vec3i({3, 2, 7}) + o);
    tris.push_back(vec3i({7, 2, 6}) + o);
    tris.push_back(vec3i({0, 4, 2}) + o);
    tris.push_back(vec3i({2, 4, 6}) + o);
    tris.push_back(vec3i({5, 1, 7}) + o);
    tris.push_back(vec3i({7, 1, 3}) + o);
    tris.push_back(vec3i({5, 4, 1}) + o);
    tris.push_back(vec3i({1, 4, 0}) + o);
    tris.push_back(vec3i({7, 6, 5}) + o);
    tris.push_back(vec3i({5, 6, 4}) + o);

    return tris;
}

template<int N>
std::vector<vec4i> get_faces(const aligned_box<N> &, size_t offset = 0) {
    std::vector<vec4i> faces;
    vec3i o(offset, offset, offset);

    faces.emplace_back(0, 1, 3, 2);
    faces.emplace_back(1, 5, 7, 3);
    faces.emplace_back(2, 3, 7, 6);
    faces.emplace_back(4, 5, 1, 0);
    faces.emplace_back(4, 0, 2, 6);
    faces.emplace_back(6, 7, 5, 4);

    return faces;
}

using aligned_box2 = aligned_box<2>;

using aligned_box3 = aligned_box<3>;

}
#endif //BCG_GRAPHICS_BCG_ALIGNED_BOX_H
