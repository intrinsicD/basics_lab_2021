//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_CAMERA_SYSTEM_H
#define BCG_GRAPHICS_BCG_CAMERA_SYSTEM_H

#include "bcg_systems.h"
#include "bcg_events.h"
#include "../bcg_viewer_state.h"

namespace bcg{

struct camera_system : public system {
    explicit camera_system(viewer_state *state);

    void on_update(const event::update &event);
};

}

#endif //BCG_GRAPHICS_BCG_CAMERA_SYSTEM_H
