//
// Created by alex on 18.11.20.
//

#ifndef BCG_GRAPHICS_BCG_BEZIER_CURVE_SYSTEM_H
#define BCG_GRAPHICS_BCG_BEZIER_CURVE_SYSTEM_H

#include "bcg_systems.h"

namespace bcg {

struct bezier_curve_system : public system {
    explicit bezier_curve_system(viewer_state *state);

    ~bezier_curve_system() override = default;

    void on_startup(const event::internal::startup &event);

    void on_enable_2d_drawing(const event::curve::enable_2d_drawing &event);

    void on_enable_3d_drawing(const event::curve::enable_3d_drawing &event);

    void on_disable_drawing(const event::curve::disable_drawing &event);

    void on_setup(const event::curve::setup &event);

    void on_make(const event::curve::make &event);

    void on_mouse_button(const event::mouse::button &event);

    void on_mouse_motion(const event::mouse::motion &event);

    const VectorS<3> &get_point() const;

    curve_mode mode;
    entt::entity current_curve_id = entt::null;
};

}

#endif //BCG_GRAPHICS_BCG_BEZIER_CURVE_SYSTEM_H
