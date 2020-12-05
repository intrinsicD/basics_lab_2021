//
// Created by alex on 04.12.20.
//

#ifndef BCG_GRAPHICS_BCG_ROTATION_OPTIMAL_H
#define BCG_GRAPHICS_BCG_ROTATION_OPTIMAL_H

#include "bcg_rotation_project_on_so.h"

namespace bcg{

template<int M, int N>
inline MatrixS<N, N> optimal_rotation(const MatrixS<M, N> &source, const MatrixS<M, N> &target, bool no_reflections = true) {
    return project_on_so<N, N>(target.transpose() * source, no_reflections);
}

}

#endif //BCG_GRAPHICS_BCG_ROTATION_OPTIMAL_H
