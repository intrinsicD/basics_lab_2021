//
// Created by alex on 01.12.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_PROPERTY_EDITFIELD_H
#define BCG_GRAPHICS_BCG_GUI_PROPERTY_EDITFIELD_H

#include "bcg_property.h"

namespace bcg{

struct viewer_state;

void edit_field(viewer_state *state, property_container *container, const std::string &name);

}
#endif //BCG_GRAPHICS_BCG_GUI_PROPERTY_EDITFIELD_H
