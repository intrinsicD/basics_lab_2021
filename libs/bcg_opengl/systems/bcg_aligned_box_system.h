//
// Created by alex on 04.11.20.
//

#ifndef BCG_GRAPHICS_BCG_ALIGNED_BOX_SYSTEM_H
#define BCG_GRAPHICS_BCG_ALIGNED_BOX_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct aligned_box_system : public system {
    explicit aligned_box_system(viewer_state *state);

    void on_add(const event::aligned_box::add &event);
};

}

#endif //BCG_GRAPHICS_BCG_ALIGNED_BOX_SYSTEM_H
