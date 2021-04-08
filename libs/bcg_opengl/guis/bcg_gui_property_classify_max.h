//
// Created by alex on 08.04.21.
//

#ifndef BCG_GRAPHICS_BCG_GUI_PROPERTY_CLASSIFY_MAX_H
#define BCG_GRAPHICS_BCG_GUI_PROPERTY_CLASSIFY_MAX_H

#include "bcg_property.h"

namespace bcg {

struct viewer_state;

struct Result{
    bool triggered = false;
    std::string current_property_name;

    operator bool(){ return triggered; }
};

Result gui_property_classify_max(viewer_state *state);

Result gui_property_classify_max(viewer_state *state, property_container *container, property<bcg_scalar_t, 1> p,
                               bcg_scalar_t &threshold, bcg_scalar_t min = -1, bcg_scalar_t max = -1);

}

#endif //BCG_GRAPHICS_BCG_GUI_PROPERTY_CLASSIFY_MAX_H
