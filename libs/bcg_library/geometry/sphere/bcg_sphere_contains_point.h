//
// Created by alex on 25.11.20.
//

#ifndef BCG_GRAPHICS_BCG_SPHERE_CONTAINS_POINT_H
#define BCG_GRAPHICS_BCG_SPHERE_CONTAINS_POINT_H

#include "sphere/bcg_sphere.h"

namespace bcg{

template <int N>
bool contains(const sphere<N> &sphere, const VectorS<N> &point){
    return (sphere.center - point).squaredNorm() <= sphere.radius * sphere.radius;
}

}

#endif //BCG_GRAPHICS_BCG_SPHERE_CONTAINS_POINT_H
