//
// Created by alex on 28.10.20.
//

#ifndef BCG_GRAPHICS_BCG_TRANSFORM_SYSTEM_H
#define BCG_GRAPHICS_BCG_TRANSFORM_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct transform_system : public system {
    explicit transform_system(viewer_state *state);

    void on_add(const event::transform::add &event);

    void on_translate(const event::transform::translate &event);

    void on_scale(const event::transform::scale &event);

    void on_rotate(const event::transform::rotate &event);

    void on_update(const event::internal::update &event);

    void on_render_gui(const event::internal::render_gui &event);
};

}

#endif //BCG_GRAPHICS_BCG_TRANSFORM_SYSTEM_H
