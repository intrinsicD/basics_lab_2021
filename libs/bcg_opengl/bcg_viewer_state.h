//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_VIEWER_CORE_H
#define BCG_GRAPHICS_BCG_VIEWER_CORE_H

#include "bcg_camera.h"
#include "bcg_library/utils/bcg_dynamic_bitset.h"

namespace bcg{

struct viewer_colors{
    VectorS<3> overlay;
    VectorS<3> entity_selection;
    VectorS<3> vertex_selection;
    VectorS<3> edge_selection;
    VectorS<3> face_selection;
    VectorS<3> boundary;
    VectorS<3> vectorfield;
    VectorS<3> wireframe;
};

struct viewer_mouse{
    dynamic_bitset buttons;
    bool is_moving, is_scrolling, is_captured_by_gui;
    VectorS<2> last_left_click, last_middle_click, last_right_click, cursor_pos;
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
