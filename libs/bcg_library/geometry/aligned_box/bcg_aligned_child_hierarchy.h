//
// Created by alex on 15.10.20.
//

#ifndef BCG_GRAPHICS_BCG_ALIGNED_CHILD_HIERARCHY_H
#define BCG_GRAPHICS_BCG_ALIGNED_CHILD_HIERARCHY_H

#include <stdint.h>

namespace bcg{

template<int N>
struct aligned_box;

template<int N>
typename aligned_box<N>::vecf child_center(const aligned_box<N> &alignedBox, uint8_t i) {
    typename aligned_box<N>::vecf childCenter, center, halfExtent;
    alignedBox.get_centered_form(center, halfExtent);
    for (unsigned int j = 0, mask = 1; j < N; ++j, mask <<= (unsigned int) 1) {
        float sign = ((i & mask) ? (float) 0.5 : (float) -0.5);
        childCenter[j] = center[j] + sign * halfExtent[j];
    }
    return childCenter;
}

template<int N>
aligned_box<N> child_box(const aligned_box<N> &alignedBox, uint8_t i) {
    typename aligned_box<N>::vecf center, halfExtent;
    alignedBox.get_centered_form(center, halfExtent);
    aligned_box<N> child;
    child.set_centered_form(child_center(alignedBox, i), halfExtent / 2.0);
    return child;
}

template<int N>
typename aligned_box<N>::vecf parent_center(const aligned_box<N> &alignedBox, uint8_t i) {
    typename aligned_box<N>::vecf parentCenter, center, halfExtent;
    alignedBox.get_centered_form(center, halfExtent);
    for (unsigned int j = 0, mask = 1; j < N; ++j, mask <<= (unsigned int) 1) {
        float sign = (i & mask ? (float) 1 : (float) -1);
        parentCenter[j] = center[j] + sign * halfExtent[j];
    }
    return parentCenter;
}

template<int N>
aligned_box<N> parent_box(const aligned_box<N> &alignedBox, uint8_t i) {
    aligned_box<N> parent;
    parent.set_centered_form(parent_center(alignedBox, i), alignedBox.halfextent() * 2);
    return parent;
}

}

#endif //BCG_GRAPHICS_BCG_ALIGNED_CHILD_HIERARCHY_H
