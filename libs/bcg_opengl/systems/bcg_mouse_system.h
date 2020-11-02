//
// Created by Alex on 02.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MOUSE_SYSTEM_H
#define BCG_GRAPHICS_BCG_MOUSE_SYSTEM_H

#include "bcg_systems.h"
#include "bcg_events.h"

namespace bcg{

struct mouse_system : public system{
    explicit mouse_system(viewer_state *state);

    void on_button(const event::mouse::button &event);
    void on_motion(const event::mouse::motion &event);
    void on_scroll(const event::mouse::scroll &event);
    void on_update(const event::update &event);
};

}

#endif //BCG_GRAPHICS_BCG_MOUSE_SYSTEM_H
