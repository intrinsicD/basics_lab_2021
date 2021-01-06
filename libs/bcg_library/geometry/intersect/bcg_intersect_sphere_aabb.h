//
// Created by alex on 06.01.21.
//

#ifndef BCG_GRAPHICS_BCG_INTERSECT_SPHERE_AABB_H
#define BCG_GRAPHICS_BCG_INTERSECT_SPHERE_AABB_H

#include "aligned_box/bcg_aligned_box.h"
#include "sphere/bcg_sphere.h"

namespace bcg{

//test if sphere intersects with aabb
template<int N>
bool intersect(const sphere<N> &sphere, const aligned_box<N> &aabb) {
    VectorS<N> xyz = (sphere.center - aabb.center()).cwiseAbs() - aabb.halfextent();
    int num_less_extent = 0;
    for(size_t i = 0; i < N; ++i){
        if(xyz[i] > sphere.radius) return false;
        if(xyz[i] < 0){
            num_less_extent += 1;
            xyz[i] = 0;
        }
    }

    if (num_less_extent > 1) return true;

    return xyz.squaredNorm() < sphere.radius * sphere.radius;
}

template<int N>
bool intersect(const aligned_box<N> &aabb, const sphere<N> &sphere) {
    return intersect<N>(sphere, aabb);
}

}

#endif //BCG_GRAPHICS_BCG_INTERSECT_SPHERE_AABB_H
