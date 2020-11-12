//
// Created by alex on 15.10.20.
//

#ifndef BCG_GRAPHICS_BCG_SPHERE_H
#define BCG_GRAPHICS_BCG_SPHERE_H

#include <iostream>
#include "math/bcg_linalg.h"

namespace bcg {

template<int N>
struct sphere {
    VectorS<N> center;
    bcg_scalar_t radius;

    sphere() : center(zeros<N>), radius(zeros<N>) {

    }

    explicit sphere(bcg_scalar_t radius) : center(zeros<N>), radius(radius) {

    }

    sphere(const VectorS<N> &center, bcg_scalar_t radius) : center(center), radius(radius) {

    }

    bcg_scalar_t volume() const {
        return std::sqrt(std::pow(pi, N)) * std::pow(radius, N) / gamma(N / 2.0 + 1);
    }

    bool operator==(sphere const &other) const {
        return (center - other.center).squaredNorm() < 2 * scalar_eps && std::pow(radius - other.radius, 2) < 2 * scalar_eps;
    }

    bool operator!=(sphere const &other) const {
        return !operator==(other);
    }

    bool operator<(sphere const &other) const {
        if (center < other.center) {
            return true;
        }

        if (center > other.center) {
            return false;
        }

        return radius < other.radius;
    }

    bool operator<=(sphere const &other) const {
        return !other.operator<(*this);
    }

    bool operator>(sphere const &other) const {
        return other.operator<(*this);
    }

    bool operator>=(sphere const &other) const {
        return !operator<(other);
    }
};

template<int N>
inline std::ostream &operator<<(std::ostream &stream, const sphere<N> &sphere) {
    stream << "center: " << sphere.center.transpose() << "\n";
    stream << "radius: " << sphere.radius << "\n";
    return stream;
}

using sphere2 = sphere<2>;

using sphere3 = sphere<3>;

}

#endif //BCG_GRAPHICS_BCG_SPHERE_H
