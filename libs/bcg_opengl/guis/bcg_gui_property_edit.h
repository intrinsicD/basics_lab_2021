//
// Created by alex on 01.04.21.
//

#ifndef BCG_GRAPHICS_BCG_GUI_PROPERTY_EDIT_H
#define BCG_GRAPHICS_BCG_GUI_PROPERTY_EDIT_H

#include "bcg_property.h"

namespace bcg{

struct viewer_state;

void gui_property_edit(viewer_state *state, property_container *container, const std::string &name);

}

#endif //BCG_GRAPHICS_BCG_GUI_PROPERTY_EDIT_H
