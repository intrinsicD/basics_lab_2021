//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_VIEWER_CORE_H
#define BCG_GRAPHICS_BCG_VIEWER_CORE_H

#include <memory>

#include "bcg_camera.h"
#include "bcg_imgui.h"
#include "bcg_opengl.h"
#include "events/bcg_events.h"
#include "color/bcg_colors.h"
#include "bcg_opengl/systems/bcg_systems.h"
#include "bcg_library/utils/bcg_dynamic_bitset.h"
#include "bcg_library/utils/bcg_file_watcher.h"
#include "bcg_library/geometry/bcg_mesh.h"
#include "entt/entt.hpp"

struct GLFWwindow;

namespace bcg {

struct viewer_colors {
    VectorS<4> background = color::default_background;
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
    bool is_moving, is_scrolling, is_dragging, is_captured_by_gui, left, middle, right;
    VectorS<2> last_left_click = zero2s;
    VectorS<2> last_middle_click = zero2s;
    VectorS<2> last_right_click = zero2s;
    VectorS<2> cursor_position = zero2s;
    VectorS<2> last_cursor_position = zero2s;
    VectorS<2> cursor_delta = zero2s;
    float scroll_value = 0;
};

struct viewer_keyboard {
    dynamic_bitset keys;
    bool shift_pressed, alt_pressed, ctrl_pressed, command_pressed, no_modifier, is_captured_by_gui;
};

struct viewer_time {
    size_t clock_now = 0;
    size_t clock_last = 0;
    double time_now = 0;
    double time_delta = 0;
};

struct viewer_state;

// Init callback called after the window has opened
using startup_callback = std::function<void(viewer_state *)>;
// Clear callback called after the window is cloased
using shutdown_callback = std::function<void(viewer_state *)>;
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

struct viewer_callbacks {
    startup_callback startup_cb = {};
    shutdown_callback shutdown_cb = {};
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

struct viewer_window {
    GLFWwindow *win = nullptr;
    std::string title = "";
    bcg_scalar_t high_dpi_scaling = 1.0;
    int width, height;
    VectorI<4> framebuffer_viewport;
    int widgets_width = 0;
};

struct viewer_gui {
    bool widgets_active = true;
    bool show_menu = true;
    bool hide_all = false;
    int menu_height = 0;
    gui_menu menu;
    left_panel left;
};

struct viewer_systems {
    std::unordered_map<std::string, std::unique_ptr<system>> systems;

    bool has(const std::string &name) const;

    std::unique_ptr<system> &operator[](const std::string &name);

    const std::unique_ptr<system> &operator[](const std::string &name) const;
};

struct viewer_shaders {
    std::unordered_map<std::string, glsl_program> programs;
    file_watcher watcher;

    glsl_program load(std::string name,
                      std::string vertex_shader_file,
                      std::string fragment_shader_file,
                      std::string *geometry_shader_file = nullptr,
                      std::string *tess_control_shader_file = nullptr,
                      std::string *tess_eval_shader_file = nullptr);

    glsl_program &operator[](const std::string &name);

    const glsl_program &operator[](const std::string &name) const;

private:
    glsl_shader load_shader(std::string filename, unsigned int type) const;
};

struct viewer_picker {
    size_t entity_id, vertex_id, edge_id, face_id;
    VectorS<3> model_space_point;
    VectorS<3> world_space_point;
    VectorS<3> view_space_point;
};

struct viewer_config {
    std::string renderers_path = "../../libs/bcg_opengl/renderers/";
};

struct viewer_state {
    viewer_state();

    viewer_config config;
    viewer_colors colors;
    viewer_keyboard keyboard;
    viewer_mouse mouse;
    viewer_picker picker;
    viewer_time time;
    viewer_callbacks callbacks;
    viewer_window window;
    viewer_gui gui;
    viewer_systems systems;
    viewer_shaders shaders;
    camera cam;
    entt::registry scene;
    entt::dispatcher dispatcher;

    vertex_container *get_vertices(entt::entity id);

    halfedge_container *get_halfedges(entt::entity id);

    edge_container *get_edges(entt::entity id);

    face_container *get_faces(entt::entity id);
};


}

#endif //BCG_GRAPHICS_BCG_VIEWER_CORE_H
