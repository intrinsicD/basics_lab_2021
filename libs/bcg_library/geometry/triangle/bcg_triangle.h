//
// Created by alex on 13.10.20.
//

#ifndef BCG_GRAPHICS_BCG_TRIANGLE_H
#define BCG_GRAPHICS_BCG_TRIANGLE_H

#include <array>
#include <ostream>
#include "math/bcg_linalg.h"
#include "bcg_triangle_metric.h"
#include "bcg_barycentric_coordinates.h"

namespace bcg {

template<int N>
struct triangle {
    std::array<VectorS<N>, 3> points;

    triangle() : points{zeros<N>, unit<N>(0), unit<N>(1)} {

    }

    triangle(const VectorS<N> &p0, const VectorS<N> &p1, const VectorS<N> &p2) : points{p0, p1, p2} {

    }

    explicit triangle(const std::array<VectorS<N>, 3> &points) : points(points) {

    }

    explicit operator std::array<VectorS<N>, 3>() const {
        return points;
    }

    inline VectorS<3> angles() const {
        auto el = edge_lengths();
        return {angle_from_metric(el[2], el[0], el[1]),
                angle_from_metric(el[0], el[1], el[2]),
                angle_from_metric(el[1], el[2], el[0])};
    };

    inline bcg_scalar_t area() const {
        return area_from_metric(edge0().norm(), edge1().norm(), edge2().norm());
    }

    inline VectorS<N> edge0() const { return points[1] - points[0]; }

    inline VectorS<N> edge1() const { return points[2] - points[1]; }

    inline VectorS<N> edge2() const { return points[0] - points[2]; }

    inline VectorS<3> edge_lengths() const {
        return {edge0().norm(), edge1().norm(), edge2().norm()};
    }

    inline bcg_scalar_t perimeter() const {
        return edge0().norm() + edge1().norm() + edge2().norm();
    }

    inline bool contains(const VectorS<N> &point) const {
        auto bc = to_barycentric_coordinates(*this, point);
        return (bc[0] >= 0 && bc[1] >= 0 && bc[2] >= 0);
    }

    std::string to_string() const {
        std::stringstream ss;
        ss << "p0: " << points[0].transpose() << "\n" <<
           "p1: " << points[1].transpose() << "\n" <<
           "p2: " << points[2].transpose() << "\n";
        return ss.str();
    }
};

template<int N>
std::ostream &operator<<(std::ostream &stream, const triangle<N> &triangle) {
    stream << triangle.to_string();
    return stream;
}

using triangle2 = triangle<2>;

using triangle3 = triangle<3>;

inline VectorS<3> normal(const triangle3 &t) {
    return (t.edge0().cross(t.edge1())).normalized();
}

}

#endif //BCG_GRAPHICS_BCG_TRIANGLE_H
