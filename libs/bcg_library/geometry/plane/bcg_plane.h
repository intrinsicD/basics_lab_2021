//
// Created by alex on 02.12.20.
//

#ifndef BCG_GRAPHICS_BCG_PLANE_H
#define BCG_GRAPHICS_BCG_PLANE_H

#include "math/vector/bcg_vector.h"
#include "math/matrix/bcg_matrix.h"
#include "math/bcg_pca.h"

namespace bcg {

template<int N>
struct Plane {
    VectorS<N> normal;
    bcg_scalar_t d;

    Plane() : normal(zeros<N>), d(0) {}

    Plane(const VectorS<N> &normal, bcg_scalar_t d) : normal(normal), d(d) {}

    Plane(const VectorS<N> &normal, const VectorS<N> &point) : normal(normal), d(normal.dot(point)) {}

    inline VectorS<N> basepoint() const {
        return normal * d;
    }

    inline bcg_scalar_t distance(const VectorS<N> &point) const {
        return (point - basepoint()).dot(normal);
    }

    inline bcg_scalar_t distance_squared(const VectorS<N> &point) const {
        bcg_scalar_t dist = distance(point);
        return dist * dist;
    }

    inline VectorS<N> project(const VectorS<N> &point) const {
        return point - normal * distance(point);
    }
};

template<int N>
std::ostream &operator<<(std::ostream &stream, const Plane<N> &plane) {
    stream << "normal: " << plane.normal.transposed() << std::endl;
    stream << "distance: " << plane.distance << std::endl;
    return stream;
}

using Plane2 = Plane<2>;

using Plane3 = Plane<3>;

}

#endif //BCG_GRAPHICS_BCG_PLANE_H
