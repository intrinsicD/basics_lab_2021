//
// Created by alex on 07.12.20.
//

#ifndef BCG_GRAPHICS_BCG_REGISTRATION_SYSTEM_H
#define BCG_GRAPHICS_BCG_REGISTRATION_SYSTEM_H

#include "bcg_systems.h"
#include "bcg_aligned_box_system.h"
#include "bcg_bezier_curve_system.h"

namespace bcg {

struct registration_system : public system {
    explicit registration_system(viewer_state *state);

    void on_align_step(const event::registration::align_step &event);

    void on_align_converge(const event::registration::align_converge &event);

    void on_reset(const event::registration::reset &event);
};

}

#endif //BCG_GRAPHICS_BCG_REGISTRATION_SYSTEM_H
