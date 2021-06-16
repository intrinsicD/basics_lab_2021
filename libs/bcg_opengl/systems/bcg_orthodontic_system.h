//
// Created by alex on 15.06.21.
//

#ifndef BCG_GRAPHICS_BCG_ORTHODONTIC_SYSTEM_H
#define BCG_GRAPHICS_BCG_ORTHODONTIC_SYSTEM_H

#include "../events/bcg_events_orthodontic.h"
#include "bcg_systems.h"

namespace bcg{

struct orthodontic_system : public system {
    explicit orthodontic_system(viewer_state *state);

    void on_mouse_button(const event::mouse::button &event);

    void on_set_tooth_component(const event::orthodontic::set_tooth_component &event);

    void on_internal_keyboard(const event::internal::keyboard &event);

    void on_reset_teeth_colors(const event::orthodontic::reset_teeth_colors &event);

    void on_set_tooth_color(const event::orthodontic::set_tooth_color &event);

    void on_render_gui(const event::orthodontic::render_gui &event);

    entt::entity jaw_id;
    int fid_number;
    bool consume_next_click;
    std::unordered_map<int, Vector<float, 3>> colors;
};

}

#endif //BCG_GRAPHICS_BCG_ORTHODONTIC_SYSTEM_H
