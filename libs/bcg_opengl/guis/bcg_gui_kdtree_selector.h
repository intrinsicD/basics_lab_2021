//
// Created by alex on 03.12.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_KDTREE_SELECTOR_H
#define BCG_GRAPHICS_BCG_GUI_KDTREE_SELECTOR_H


#include "math/bcg_math_common.h"

namespace bcg{

struct viewer_state;

struct kdtree_parameters{
    enum class Type{
        knn,
        radius
    }type;
    int num_closest = 12;
    bcg_scalar_t radius = 0.001;
};

kdtree_parameters gui_kd_tree_selector(viewer_state *state);

}

#endif //BCG_GRAPHICS_BCG_GUI_KDTREE_SELECTOR_H
