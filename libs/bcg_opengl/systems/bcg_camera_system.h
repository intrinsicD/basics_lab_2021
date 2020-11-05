//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_CAMERA_SYSTEM_H
#define BCG_GRAPHICS_BCG_CAMERA_SYSTEM_H

#include "bcg_systems.h"

namespace bcg{

struct camera_system : public system {
    explicit camera_system(viewer_state *state);

    void on_startup(const event::internal::startup &event);

    void on_resize(const event::internal::resize &event);

    void on_mouse_scroll(const event::mouse::scroll &event);

    void on_update(const event::internal::update &event);

    void on_end_frame(const event::internal::end_frame &event);
};

}

#endif //BCG_GRAPHICS_BCG_CAMERA_SYSTEM_H
