//
// Created by alex on 15.10.20.
//

#ifndef BCG_GRAPHICS_BCG_ALIGNED_CHILD_HIERARCHY_H
#define BCG_GRAPHICS_BCG_ALIGNED_CHILD_HIERARCHY_H

#include <stdint.h>
#include "bcg_aligned_box.h"
#include "math/vector/bcg_vector.h"

namespace bcg{

inline VectorS<3> child_center(const aligned_box3 &aabb, uint8_t i) {
    VectorS<3> childCenter, center, halfExtent;
    aabb.get_centered_form(center, halfExtent);
    for (unsigned int j = 0, mask = 1; j < 3; ++j, mask <<= (unsigned int) 1) {
        auto sign = ((i & mask) ? 0.5 : -0.5);
        childCenter[j] = center[j] + sign * halfExtent[j];
    }
    return childCenter;
}

inline aligned_box3 child_box(const aligned_box3 &aabb, uint8_t i) {
    VectorS<3> center, halfExtent;
    aabb.get_centered_form(center, halfExtent);
    aligned_box3 child;
    child.set_centered_form(child_center(aabb, i), halfExtent / 2.0);
    return child;
}

inline VectorS<3> parent_center(const aligned_box3 &aabb, uint8_t i) {
    VectorS<3> parentCenter, center, halfExtent;
    aabb.get_centered_form(center, halfExtent);
    for (unsigned int j = 0, mask = 1; j < 3; ++j, mask <<= (unsigned int) 1) {
        auto sign = (i & mask ? 1 : -1);
        parentCenter[j] = center[j] + sign * halfExtent[j];
    }
    return parentCenter;
}

inline aligned_box3 parent_box(const aligned_box3 &alignedBox, uint8_t i) {
    aligned_box3 parent;
    parent.set_centered_form(parent_center(alignedBox, i), alignedBox.halfextent() * 2);
    return parent;
}

}

#endif //BCG_GRAPHICS_BCG_ALIGNED_CHILD_HIERARCHY_H
