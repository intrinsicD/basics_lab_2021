//
// Created by alex on 25.03.21.
//

#ifndef BCG_GRAPHICS_BCG_PARAMETERIZED_PLANE_H
#define BCG_GRAPHICS_BCG_PARAMETERIZED_PLANE_H

#include "math/vector/bcg_vector.h"

namespace bcg {

template<int N>
struct ParameterizedPlane {
    VectorS<N> base_point;
    MatrixS<N, 2> directions;

    ParameterizedPlane() : base_point(zeros<N>), directions() {
        directions.col(0) = unit<N>(0);
        directions.col(1) = unit<N>(1);
    }

    explicit ParameterizedPlane(const VectorS<N> &base_point) : base_point(base_point), directions() {
        directions.col(0) = unit<N>(0);
        directions.col(1) = unit<N>(1);
    }

    ParameterizedPlane(const VectorS<N> &base_point, const MatrixS<N, 2> &directions) : base_point(
            base_point), directions(directions) {}

    ParameterizedPlane(const VectorS<N> &base_point, const std::array<VectorS<N>, 2> &directions) : base_point(
            base_point), directions() {
        directions.col(0) = directions[0];
        directions.col(1) = directions[1];
    }

    inline VectorS<N> plane_point(bcg_scalar_t x, bcg_scalar_t y) const {
        return plane_point(VectorS<2>(x, y));
    }

    inline VectorS<N> plane_point(const VectorS<2> &xy) const {
        return base_point + directions * xy;
    }

    inline VectorS<2> parameters(const VectorS<N> &point) const {
        return (point - base_point).transpose() * directions;
    }

    inline VectorS<N> project(const VectorS<N> &point) const {
        return plane_point(parameters(point));
    }

    inline bcg_scalar_t distance_squared(const VectorS<N> &point) const {
        return (point - project(point)).squaredNorm();
    }

    inline bcg_scalar_t distance(const VectorS<N> &point) const {
        return (point - project(point)).norm();
    }
};

template<int N>
std::ostream &operator<<(std::ostream &stream, const ParameterizedPlane<N> &plane) {
    stream << "ParameterizedPlane<" << N << ">:\n";
    stream << "base_point: " << plane.base_point.transpose() << "\n";
    stream << "direction1: " << plane.directions.col(0).transpose() << "\n";
    stream << "direction2: " << plane.directions.col(1).transpose() << "\n";
    return stream;
}

inline VectorS<3> normal(const ParameterizedPlane<3> &plane) {
    return plane.directions.col(0).cross(plane.directions.col(1)).normalized();
}

inline bcg_scalar_t signed_distance(const ParameterizedPlane<3> &plane, const VectorS<3> &point) {
    return (point - plane.base_point).dot(normal(plane));
}

template<int N>
std::vector<VectorS<N>> get_vertices(const ParameterizedPlane<N> &plane) {
    std::vector<VectorS<N>> vertices(4);
    vertices[0] = plane.base_point - plane.directions.col(0) - plane.directions.col(1);
    vertices[1] = plane.base_point + plane.directions.col(0) - plane.directions.col(1);
    vertices[2] = plane.base_point + plane.directions.col(0) + plane.directions.col(1);
    vertices[3] = plane.base_point - plane.directions.col(0) + plane.directions.col(1);
    return vertices;
}

template<int N>
std::vector<VectorS<N>> get_edges(const ParameterizedPlane<N> &) {
    return {VectorI<2>(0, 1),
            VectorI<2>(1, 2),
            VectorI<2>(2, 3),
            VectorI<2>(3, 0)};

}

template<int N>
std::vector<VectorI<3>> get_faces(const ParameterizedPlane<N> &) {
    return {VectorI<3>(0, 1, 2), VectorI<3>(0, 2, 3)};
}

using ParameterizedPlane2 = ParameterizedPlane<2>;

using ParameterizedPlane3 = ParameterizedPlane<3>;
}

#endif //BCG_GRAPHICS_BCG_PARAMETERIZED_PLANE_H
