//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_VIEWER_CORE_H
#define BCG_GRAPHICS_BCG_VIEWER_CORE_H

#include "bcg_camera.h"
#include "bcg_library/utils/bcg_dynamic_bitset.h"

namespace bcg{

struct viewer_colors{
    vec3f overlay;
    vec3f entity_selection;
    vec3f vertex_selection;
    vec3f edge_selection;
    vec3f face_selection;
    vec3f boundary;
    vec3f vectorfield;
    vec3f wireframe;
};

struct viewer_mouse{
    dynamic_bitset buttons;
    bool is_moving, is_scrolling, is_captured_by_gui;
    vec2f last_left_click, last_middle_click, last_right_click, cursor_pos;
    float scroll_value;
};

struct viewer_keyboard{
    dynamic_bitset keys;
    bool shift_pressed, alt_pressed, ctrl_pressed, command_pressed, no_modifier, is_captured_by_gui;
};

struct viewer_state{
    viewer_colors colors;
    viewer_keyboard keyboard;
    viewer_mouse mouse;
    camera cam;
};

}

#endif //BCG_GRAPHICS_BCG_VIEWER_CORE_H
