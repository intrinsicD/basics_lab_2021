//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_LOADING_SYSTEM_H
#define BCG_GRAPHICS_BCG_LOADING_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct loading_system : public system {
    explicit loading_system(viewer_state *state);

    void on_file_drop(const event::internal::file_drop &event);
};

}

#endif //BCG_GRAPHICS_BCG_LOADING_SYSTEM_H
