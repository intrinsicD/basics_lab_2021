//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_PROPERTY_CONTAINER_SELECTOR_H
#define BCG_GRAPHICS_BCG_GUI_PROPERTY_CONTAINER_SELECTOR_H

#include "bcg_property.h"

namespace bcg{

struct viewer_state;

void gui_property_container_selector(viewer_state *state, property_container *container, size_t &current_entry);

}

#endif //BCG_GRAPHICS_BCG_GUI_PROPERTY_CONTAINER_SELECTOR_H
