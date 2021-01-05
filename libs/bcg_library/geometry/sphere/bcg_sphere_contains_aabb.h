//
// Created by alex on 05.01.21.
//

#ifndef BCG_GRAPHICS_BCG_SPHERE_CONTAINS_AABB_H
#define BCG_GRAPHICS_BCG_SPHERE_CONTAINS_AABB_H

#include "bcg_sphere.h"
#include "aligned_box/bcg_aligned_box.h"

namespace bcg {

//test if sphere contains aabb
template<int N>
bool contains(const sphere <N> &sphere, const aligned_box <N> &aabb) {
    return ((sphere.center - aabb.center()).cwiseAbs() + aabb.halfextent()).squaredNorm() <
           sphere.radius * sphere.radius;
}

}
#endif //BCG_GRAPHICS_BCG_SPHERE_CONTAINS_AABB_H
