//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_PROPERTY_SELECTOR_H
#define BCG_GRAPHICS_BCG_GUI_PROPERTY_SELECTOR_H

#include <string>
#include <vector>

namespace bcg{

struct viewer_state;
struct property_container;

bool gui_property_selector(viewer_state *state,
                                property_container *container,
                                const std::vector<int> &filter_dims,
                                std::string label,
                                std::string &current_property_name);

}

#endif //BCG_GRAPHICS_BCG_GUI_PROPERTY_SELECTOR_H
