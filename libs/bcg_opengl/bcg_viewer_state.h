//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_VIEWER_CORE_H
#define BCG_GRAPHICS_BCG_VIEWER_CORE_H

#include "bcg_camera.h"
#include "bcg_picker.h"
#include "bcg_library/utils/bcg_dynamic_bitset.h"

struct GLFWwindow;

namespace bcg {

struct viewer_colors {
    VectorS<3> overlay;
    VectorS<3> entity_selection;
    VectorS<3> vertex_selection;
    VectorS<3> edge_selection;
    VectorS<3> face_selection;
    VectorS<3> boundary;
    VectorS<3> wireframe;
};

struct viewer_mouse {
    dynamic_bitset buttons;
    bool is_moving, is_scrolling, is_captured_by_gui;
    VectorS<2> last_left_click, last_middle_click, last_right_click, cursor_pos;
    float scroll_value;
};

struct viewer_keyboard {
    dynamic_bitset keys;
    bool shift_pressed, alt_pressed, ctrl_pressed, command_pressed, no_modifier, is_captured_by_gui;
};

struct viewer_time{
    size_t clock_now = 0;
    size_t clock_last = 0;
    double time_now = 0;
    double time_delta = 0;
};

struct viewer_state;

// Init callback called after the window has opened
using init_callback = std::function<void(viewer_state *)>;
// Clear callback called after the window is cloased
using clear_callback = std::function<void(viewer_state *)>;
// Draw callback called every frame and when resizing
using draw_callback = std::function<void(viewer_state *)>;
// Draw callback for drawing widgets
using widgets_callback = std::function<void(viewer_state *)>;
// Drop callback that returns that list of dropped std::strings.
using drop_callback = std::function<void(viewer_state *, const std::vector<std::string> &)>;
// Key callback that returns key codes, pressed/released flag and modifier keys
using key_callback = std::function<void(viewer_state *, int key, bool pressed)>;
// Char callback that returns ASCII key
using char_callback = std::function<void(viewer_state *, unsigned int key)>;
// Mouse click callback that returns left/right button, pressed/released flag,
// modifier keys
using click_callback = std::function<void(viewer_state *, bool left, bool pressed)>;
// Scroll callback that returns scroll amount
using scroll_callback = std::function<void(viewer_state *, float amount)>;
// Update std::functions called every frame
using uiupdate_callback = std::function<void(viewer_state *)>;
// Update std::functions called every frame
using update_callback = std::function<void(viewer_state *)>;

struct viewer_callbacks{
    init_callback init_cb = {};
    clear_callback clear_cb = {};
    draw_callback draw_cb = {};
    widgets_callback widgets_cb = {};
    drop_callback drop_cb = {};
    key_callback key_cb = {};
    char_callback char_cb = {};
    click_callback click_cb = {};
    scroll_callback scroll_cb = {};
    update_callback update_cb = {};
    uiupdate_callback uiupdate_cb = {};
};

struct viewer_window{
    GLFWwindow *win = nullptr;
    std::string title = "";
    int width, height;
};

struct viewer_state {
    viewer_colors colors;
    viewer_keyboard keyboard;
    viewer_mouse mouse;
    viewer_picker picker;
    viewer_time time;
    viewer_callbacks callbacks;
    viewer_window window;
    camera cam;
};

}

#endif //BCG_GRAPHICS_BCG_VIEWER_CORE_H
