//
// Created by alex on 14.10.20.
//

#ifndef BCG_GRAPHICS_BCG_QUAD_H
#define BCG_GRAPHICS_BCG_QUAD_H

#include "../triangle/bcg_triangle.h"

namespace bcg {

template<bcg_index_t N>
struct quad {
    std::array<VectorS<N>, 4> points;

    quad() : points{zeros<N>, zeros<N>, zeros<N>, zeros<N>}{
        points[0][0] = 0.5;
        points[0][1] = 0.5;
        points[1][0] = 0.5;
        points[1][1] = -0.5;
        points[2][0] = -0.5;
        points[2][1] = -0.5;
        points[3][0] = -0.5;
        points[3][1] = 0.5;
    }

    quad(const VectorS<N> &p0, const VectorS<N> &p1, const VectorS<N> &p2, const VectorS<N> &p3) : points{p0, p1, p2, p3} {

    }

    explicit quad(const std::array<VectorS<N>, 4> &points) : points(points) {

    }

    explicit operator std::array<VectorS<N>, 4>() const {
        return points;
    }

    inline VectorS<3> angles() const {
        auto a0 = triangle0().angles();
        auto a1 = triangle1().angles();
        return {a0[0], a0[1] + a1[0], a1[1], a1[2] + a0[2]};
    };

    inline bcg_scalar_t area() const {
        return triangle0().area() + triangle1().area();
    }

    inline VectorS<N> edge0() const { return points[1] - points[0]; }

    inline VectorS<N> edge1() const { return points[3] - points[1]; }

    inline VectorS<N> edge2() const { return points[2] - points[3]; }

    inline VectorS<N> edge3() const { return points[0] - points[2]; }

    inline VectorS<N> diagonal() const { return points[2] - points[1]; }

    inline VectorS<N> offdiagonal() const { return points[3] - points[0]; }

    inline triangle <N> triangle0() const {
        return triangle<N>(points[0], points[1], points[2]);
    }

    inline triangle <N> triangle1() const {
        return triangle<N>(points[1], points[3], points[2]);
    }

    inline VectorS<4> edge_lengths() const {
        return {length(edge0()), length(edge1()), length(edge2()), length(edge3())};
    }

    inline bcg_scalar_t perimeter() const {
        return length(edge0()) + length(edge1()) + length(edge2()) + length(edge3());
    }

    inline bool contains(const VectorS<N> &point) const {
        return triangle0().contains(point) || triangle1().contains(point);
    }

    std::string to_string() const {
        std::string output = "p0: " + points[0].transport() + "\n" +
                             "p1: " + points[1].transport() + "\n" +
                             "p2: " + points[2].transport() + "\n" +
                             "p3: " + points[3].transport() + "\n" +
                             "diagonal split from p1 to p2 - z order!\n";
        return output;
    }
};

template<bcg_index_t N>
std::ostream &operator<<(std::ostream &stream, const quad<N> &quad) {
    stream << quad.to_string();
    return stream;
}

using quad2 = quad<2>;

using quad3 = quad<3>;

inline VectorS<3> normal(const quad3 &t) {
    return t.diagonal().cross(t.offdiagonal()).normalized();
}

}

#endif //BCG_GRAPHICS_BCG_QUAD_H
