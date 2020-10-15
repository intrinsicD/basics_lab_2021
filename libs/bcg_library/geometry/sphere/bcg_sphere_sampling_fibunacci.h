//
// Created by alex on 24.09.20.
//

#ifndef BCG_SPHERESAMPLINGFIBUNACCI_H
#define BCG_SPHERESAMPLINGFIBUNACCI_H

#include <vector>
#include "bcg_sphere.h"

namespace bcg {

enum FibunacciLattice {
    FLNAIVE,
    FLFIRST,
    FLSECOND,
    FLTHIRD
};

std::vector<VectorS<3>>
sphere_fibunacci_sampling(const sphere3 &sphere, size_t n, FibunacciLattice type = FLTHIRD) {
    //http://extremelearning.com.au/evenly-distributing-points-on-a-sphere/
    std::vector<VectorS<3>> points;
    points.reserve(n);
    bcg_scalar_t golden_ratio = (1.0 + std::sqrt(5.0)) / 2;
    bcg_scalar_t TWOPI = 2 * M_PI;

    switch (type) {
        default:
        case FLNAIVE: {
            for (size_t i = 0; i < n; ++i) {
                bcg_scalar_t x = i / bcg_scalar_t(n);
                bcg_scalar_t y = i / golden_ratio;
                bcg_scalar_t phi = std::acos(1.0 - 2.0 * x);
                bcg_scalar_t theta = TWOPI * y;
                VectorS<3> point = {std::cos(theta) * std::sin(phi), std::sin(theta) * std::sin(phi), std::cos(phi)};
                points.emplace_back(point * sphere.radius + sphere.center);
            }
            break;
        }
        case FLFIRST: {
            for (size_t i = 0; i < n; ++i) {
                bcg_scalar_t x = (i + 0.5) / bcg_scalar_t(n);
                bcg_scalar_t y = i / golden_ratio;
                bcg_scalar_t phi = std::acos(1.0 - 2.0 * x);
                bcg_scalar_t theta = TWOPI * y;
                VectorS<3> point = {std::cos(theta) * std::sin(phi), std::sin(theta) * std::sin(phi), std::cos(phi)};
                points.emplace_back(point * sphere.radius + sphere.center);
            }
            break;
        }
        case FLSECOND: {
            for (size_t i = 0; i < n; ++i) {
                bcg_scalar_t x = (i + 1.5) / bcg_scalar_t(n + 3);
                bcg_scalar_t y = i / golden_ratio;
                bcg_scalar_t phi = std::acos(1.0 - 2.0 * x);
                bcg_scalar_t theta = TWOPI * y;
                VectorS<3> point = {std::cos(theta) * std::sin(phi), std::sin(theta) * std::sin(phi), std::cos(phi)};
                points.emplace_back(point * sphere.radius + sphere.center);
            }
            break;
        }
        case FLTHIRD: {
            VectorS<3> point = {0, 0, 1};
            points.emplace_back(point * sphere.radius + sphere.center);
            for (size_t i = 1; i < n - 1; ++i) {
                bcg_scalar_t x = (i + 3.5) / bcg_scalar_t(n + 6);
                bcg_scalar_t y = i / golden_ratio;
                bcg_scalar_t phi = std::acos(1.0 - 2.0 * x);
                bcg_scalar_t theta = TWOPI * y;
                point = {std::cos(theta) * std::sin(phi), std::sin(theta) * std::sin(phi), std::cos(phi)};
                points.emplace_back(point * sphere.radius + sphere.center);
            }
            point = {0, 0, -1};
            points.emplace_back(point * sphere.radius + sphere.center);
            break;
        }
    }
    return points;
}

}

#endif //BCG_SPHERESAMPLINGFIBUNACCI_H
