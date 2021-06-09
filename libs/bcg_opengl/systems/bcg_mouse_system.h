//
// Created by Alex on 02.11.20.
//

#ifndef BCG_GRAPHICS_BCG_MOUSE_SYSTEM_H
#define BCG_GRAPHICS_BCG_MOUSE_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct mouse_system : public system {
    explicit mouse_system(viewer_state *state);

private:
    void on_button(const event::internal::mouse::button &event);

    void on_motion(const event::internal::mouse::motion &event);

    void on_scroll(const event::internal::mouse::scroll &event);

    void on_end_frame(const event::internal::end_frame &event);
};

}

#endif //BCG_GRAPHICS_BCG_MOUSE_SYSTEM_H
