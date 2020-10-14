//
// Created by alex on 13.10.20.
//

#ifndef BCG_GRAPHICS_BCG_TRIANGLE_H
#define BCG_GRAPHICS_BCG_TRIANGLE_H

#include <array>
#include <ostream>
#include "../../math/bcg_math_common.h"
#include "../../math/bcg_linalg.h"
#include "bcg_triangle_metric.h"
#include "bcg_barycentric_coordinates.h"

namespace bcg {

template<int N>
struct triangle {
    using vecf = vec<N, float>;
    std::array<vecf, 3> points;

    triangle(const vecf &p0, const vecf &p1, const vecf &p2) : points{p0, p1, p2} {

    }

    explicit triangle(const std::array<vec3f, 3> &points) : points(points) {

    }

    operator std::array<vecf, 3>() const {
        return points;
    }

    inline vec3 angles() const {
        auto el = edge_lengths();
        return {angle_from_metric(el[2], el[0], el[1]),
                angle_from_metric(el[0], el[1], el[2]),
                angle_from_metric(el[1], el[2], el[0])};
    };

    inline float area() const {
        return area_from_metric(length(edge0()), length(edge1()), length(edge2()));
    }

    inline vecf edge0() const { return points[1] - points[0]; }

    inline vecf edge1() const { return points[2] - points[1]; }

    inline vecf edge2() const { return points[0] - points[2]; }

    inline vec3f edge_lengths() const {
        return {length(edge0()), length(edge1()), length(edge2())};
    }

    inline float perimeter() const {
        return length(edge0()) + length(edge1()) + length(edge2());
    }

    inline bool contains(const vecf &point) const {
        auto bc = barycentric_coordinates(*this, point);
        return (bc[0] >= 0 && bc[1] >= 0 && bc[2] >= 0);
    }

    std::string to_string() const {
        std::string output = "p0: " + glm::to_string(points[0]) + "\n" +
                             "p1: " + glm::to_string(points[1]) + "\n" +
                             "p2: " + glm::to_string(points[2]) + "\n";
    }
};

template<int N>
std::ostream &operator<<(std::ostream &stream, const triangle<N> &triangle) {
    stream << triangle.to_string();
    return stream;
}

using triangle2 = triangle<2>;

using triangle3 = triangle<3>;

inline vec3f normal(const triangle3 &t) {
    return normalize(cross(t.edge0(), t.edge1()));
}

}

#endif //BCG_GRAPHICS_BCG_TRIANGLE_H
