//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_PICKER_SYSTEM_H
#define BCG_GRAPHICS_BCG_PICKER_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct picker_system : public system {
    explicit picker_system(viewer_state *state);
};

}

#endif //BCG_GRAPHICS_BCG_PICKER_SYSTEM_H
