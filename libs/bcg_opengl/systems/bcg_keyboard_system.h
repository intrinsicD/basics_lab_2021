//
// Created by Alex on 02.11.20.
//

#ifndef BCG_GRAPHICS_BCG_KEYBOARD_SYSTEM_H
#define BCG_GRAPHICS_BCG_KEYBOARD_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct keyboard_system : public system {
    explicit keyboard_system(viewer_state *state);

    void on_keyboard(const event::keyboard &event);

    void on_update(const event::update &event);
};


}

#endif //BCG_GRAPHICS_BCG_KEYBOARD_SYSTEM_H
