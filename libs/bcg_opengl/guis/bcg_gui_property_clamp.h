//
// Created by alex on 08.04.21.
//

#ifndef BCG_GRAPHICS_BCG_GUI_PROPERTY_CLAMP_H
#define BCG_GRAPHICS_BCG_GUI_PROPERTY_CLAMP_H

#include "bcg_property.h"

namespace bcg{

struct viewer_state;

void gui_property_clamp(viewer_state *state, property_container *container, property<bcg_scalar_t, 1> p, float &min, float &max);

}

#endif //BCG_GRAPHICS_BCG_GUI_PROPERTY_CLAMP_H
