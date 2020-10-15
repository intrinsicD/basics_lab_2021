//
// Created by alex on 14.10.20.
//

#ifndef BCG_GRAPHICS_BCG_QUAD_H
#define BCG_GRAPHICS_BCG_QUAD_H

#include "../triangle/bcg_triangle.h"

namespace bcg {

template<int N>
struct quad {
    using vecf = vec<N, float>;
    std::array<vecf, 4> points;

    quad(const vecf &p0, const vecf &p1, const vecf &p2, const vecf &p3) : points{p0, p1, p2, p3} {

    }

    explicit quad(const std::array<vecf, 4> &points) : points(points) {

    }

    explicit operator std::array<vecf, 4>() const {
        return points;
    }

    inline vec3 angles() const {
        auto a0 = triangle0().angles();
        auto a1 = triangle1().angles();
        return {a0[0], a0[1] + a1[0], a1[1], a1[2] + a0[2]};
    };

    inline float area() const {
        return triangle0().area() + triangle1().area();
    }

    inline vecf edge0() const { return points[1] - points[0]; }

    inline vecf edge1() const { return points[3] - points[1]; }

    inline vecf edge2() const { return points[2] - points[3]; }

    inline vecf edge3() const { return points[0] - points[2]; }

    inline vecf diagonal() const { return points[2] - points[1]; }

    inline vecf offdiagonal() const { return points[3] - points[0]; }

    inline triangle <N> triangle0() const {
        return triangle<N>(points[0], points[1], points[2]);
    }

    inline triangle <N> triangle1() const {
        return triangle<N>(points[1], points[3], points[2]);
    }

    inline vec4f edge_lengths() const {
        return {length(edge0()), length(edge1()), length(edge2()), length(edge3())};
    }

    inline float perimeter() const {
        return length(edge0()) + length(edge1()) + length(edge2()) + length(edge3());
    }

    inline bool contains(const vecf &point) const {
        return triangle0().contains(point) || triangle1().contains(point);
    }

    std::string to_string() const {
        std::string output = "p0: " + glm::to_string(points[0]) + "\n" +
                             "p1: " + glm::to_string(points[1]) + "\n" +
                             "p2: " + glm::to_string(points[2]) + "\n" +
                             "p3: " + glm::to_string(points[3]) + "\n" +
                             "diagonal split from p1 to p2 - z order!\n";
        return output;
    }
};

template<int N>
std::ostream &operator<<(std::ostream &stream, const quad<N> &quad) {
    stream << quad.to_string();
    return stream;
}

using quad2 = quad<2>;

using quad3 = quad<3>;

inline vec3f normal(const quad3 &t) {
    return normalize(cross(t.diagonal(), t.offdiagonal()));
}

}

#endif //BCG_GRAPHICS_BCG_QUAD_H
