//
// Created by alex on 06.01.21.
//

#ifndef BCG_GRAPHICS_BCG_ALIGNED_BOX_CONTAINS_SPHERE_H
#define BCG_GRAPHICS_BCG_ALIGNED_BOX_CONTAINS_SPHERE_H

#include "bcg_aligned_box.h"
#include "sphere/bcg_sphere.h"
namespace bcg{

//test if aabb contains sphere
template<int N>
bool contains(const aligned_box<N> &aabb, const sphere<N> &sphere) {
    VectorS<3> xyz = ((sphere.center - aabb.center()).cwiseAbs().array() + sphere.radius).matrix() - aabb.halfextent();
    if (xyz[0] > 0) return false;
    if (xyz[1] > 0) return false;
    if (xyz[2] > 0) return false;
    return true;
}

}
#endif //BCG_GRAPHICS_BCG_ALIGNED_BOX_CONTAINS_SPHERE_H
