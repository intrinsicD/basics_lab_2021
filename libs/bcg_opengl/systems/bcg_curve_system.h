//
// Created by alex on 13.11.20.
//

#ifndef BCG_GRAPHICS_BCG_CURVE_SYSTEM_H
#define BCG_GRAPHICS_BCG_CURVE_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct curve_system : public system {
    explicit curve_system(viewer_state *state);

    void on_startup(const event::internal::startup &event);

    void on_disable_mode(const event::curve::disable_curve_mode &event);

    void on_enable_plane_mode(const event::curve::enable_plane_curve_mode &event);

    void on_enable_surface_mode(const event::curve::enable_surface_curve_mode &event);

    void on_setup_curve(const event::curve::setup_curve &event);

    void on_make_curve(const event::curve::new_curve &event);

    void on_delete_curve(const event::curve::delete_curve &event);

    void on_mouse_button(const event::mouse::button &event);

    void on_mouse_motion(const event::mouse::motion &event);

private:
    curve_mode mode;
};

}

#endif //BCG_GRAPHICS_BCG_CURVE_SYSTEM_H
