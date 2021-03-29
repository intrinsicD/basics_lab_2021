//
// Created by alex on 29.03.21.
//

#ifndef BCG_GRAPHICS_BCG_GUI_RADIUS_FROM_AABB_H
#define BCG_GRAPHICS_BCG_GUI_RADIUS_FROM_AABB_H

#include "math/bcg_math_common.h"

namespace bcg{

struct viewer_state;

bcg_scalar_t gui_radius_from_aabb(viewer_state *state, bool *changed = nullptr);

}


#endif //BCG_GRAPHICS_BCG_GUI_RADIUS_FROM_AABB_H
