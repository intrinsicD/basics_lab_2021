//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_TRANSFORM_H
#define BCG_GRAPHICS_BCG_GUI_TRANSFORM_H

#include "math/bcg_linalg.h"

namespace bcg{

struct viewer_state;

void gui_transform(viewer_state *state, Transform *transform, const std::string *name = nullptr);

}

#endif //BCG_GRAPHICS_BCG_GUI_TRANSFORM_H
