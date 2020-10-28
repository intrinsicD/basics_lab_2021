//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_MESH_SYSTEM_H
#define BCG_GRAPHICS_BCG_MESH_SYSTEM_H

#include "bcg_systems.h"
#include "bcg_events.h"

namespace bcg{

struct mesh_system : public system{
    explicit mesh_system(viewer_state *state);

    void on_file_drop(const event::file_drop &event);
};

}

#endif //BCG_GRAPHICS_BCG_MESH_SYSTEM_H
