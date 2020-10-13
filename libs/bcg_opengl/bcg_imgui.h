#ifndef BCG_GRAPHICS_BCG_IMGUI
#define BCG_GRAPHICS_BCG_IMGUI

// -----------------------------------------------------------------------------
// INCLUDES
// -----------------------------------------------------------------------------

#include <functional>
#include <string>
#include <vector>

#include "bcg_library/math/bcg_linalg.h"

// forward declaration
struct GLFWwindow;

// -----------------------------------------------------------------------------
// UI APPLICATION
// -----------------------------------------------------------------------------
namespace bcg {

// Forward declaration of OpenGL window
struct gui_window;

// Input state
struct gui_input {
    bool mouse_left = false;  // left button
    bool mouse_right = false;  // right button
    bool mouse_middle = false;  // middle button
    vec2f mouse_pos = {};     // position excluding widgets
    vec2f mouse_last = {};  // last mouse position excluding widgets
    vec2f mouse_delta = {};  // last mouse delta excluding widgets
    bool modifier_alt = false;         // alt modifier
    bool modifier_ctrl = false;         // ctrl modifier
    bool modifier_shift = false;         // shift modifier
    bool widgets_active = false;         // widgets are active
    uint64_t clock_now = 0;             // clock now
    uint64_t clock_last = 0;             // clock last
    double time_now = 0;             // time now
    double time_delta = 0;             // time delta
    vec2i window_size = {0, 0};        // window size
    vec4i framebuffer_viewport = {0, 0, 0, 0};  // framebuffer viewport
};

// Init callback called after the window has opened
using init_callback = std::function<void(gui_window *, const gui_input &input)>;
// Clear callback called after the window is cloased
using clear_callback = std::function<void(gui_window *, const gui_input &input)>;
// Draw callback called every frame and when resizing
using draw_callback = std::function<void(gui_window *, const gui_input &input)>;
// Draw callback for drawing widgets
using widgets_callback = std::function<void(gui_window *, const gui_input &input)>;
// Drop callback that returns that list of dropped std::strings.
using drop_callback = std::function<void(gui_window *, const std::vector<std::string> &, const gui_input &input)>;
// Key callback that returns key codes, pressed/released flag and modifier keys
using key_callback = std::function<void(gui_window *, int key, bool pressed, const gui_input &input)>;
// Char callback that returns ASCII key
using char_callback = std::function<void(gui_window *, unsigned int key, const gui_input &input)>;
// Mouse click callback that returns left/right button, pressed/released flag,
// modifier keys
using click_callback = std::function<void(gui_window *, bool left, bool pressed, const gui_input &input)>;
// Scroll callback that returns scroll amount
using scroll_callback = std::function<void(gui_window *, float amount, const gui_input &input)>;
// Update std::functions called every frame
using uiupdate_callback = std::function<void(gui_window *, const gui_input &input)>;
// Update std::functions called every frame
using update_callback = std::function<void(gui_window *, const gui_input &input)>;

// User interface callcaks
struct gui_callbacks {
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

// run the user interface with the given callbacks
void run_ui(const vec2i &size, const std::string &title,
            const gui_callbacks &callbaks, int widgets_width = 320,
            bool widgets_left = true);

}  // namespace bcg

// -----------------------------------------------------------------------------
// UI WINDOW
// -----------------------------------------------------------------------------
namespace bcg {

// OpenGL window wrapper
struct gui_window {
    GLFWwindow *win = nullptr;
    std::string title = "";
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
    int widgets_width = 0;
    bool widgets_left = true;
    gui_input input = {};
    vec4f background = {0.15f, 0.15f, 0.15f, 1.0f};
};

// Windows initialization
void init_window(gui_window *win, const vec2i &size, const std::string &title,
                 bool widgets, int widgets_width = 320, bool widgets_left = true);

// Window cleanup
void clear_window(gui_window *win);

// Set callbacks
void set_init_callback(gui_window *win, init_callback init_cb);

void set_clear_callback(gui_window *win, clear_callback clear_cb);

void set_draw_callback(gui_window *win, draw_callback draw_cb);

void set_widgets_callback(gui_window *win, widgets_callback widgets_cb);

void set_drop_callback(gui_window *win, drop_callback drop_cb);

void set_key_callback(gui_window *win, key_callback cb);

void set_char_callback(gui_window *win, char_callback cb);

void set_click_callback(gui_window *win, click_callback cb);

void set_scroll_callback(gui_window *win, scroll_callback cb);

void set_uiupdate_callback(gui_window *win, uiupdate_callback cb);

void set_update_callback(gui_window *win, update_callback cb);

// Run loop
void run_ui(gui_window *win);

void set_close(gui_window *win, bool close);

}  // namespace bcg

// -----------------------------------------------------------------------------
// OPENGL WIDGETS
// -----------------------------------------------------------------------------
namespace bcg {

bool begin_header(gui_window *win, const char *title);

void end_header(gui_window *win);

void draw_label(gui_window *win, const char *lbl, const std::string &text);

void draw_separator(gui_window *win);

void continue_line(gui_window *win);

bool draw_button(gui_window *win, const char *lbl, bool enabled = true);

bool draw_textinput(gui_window *win, const char *lbl, std::string &value);

bool draw_slider(
        gui_window *win, const char *lbl, float &value, float min, float max);

bool draw_slider(
        gui_window *win, const char *lbl, vec2f &value, float min, float max);

bool draw_slider(
        gui_window *win, const char *lbl, vec3f &value, float min, float max);

bool draw_slider(
        gui_window *win, const char *lbl, vec4f &value, float min, float max);

bool draw_slider(
        gui_window *win, const char *lbl, int &value, int min, int max);

bool draw_slider(
        gui_window *win, const char *lbl, vec2i &value, int min, int max);

bool draw_slider(
        gui_window *win, const char *lbl, vec3i &value, int min, int max);

bool draw_slider(
        gui_window *win, const char *lbl, vec4i &value, int min, int max);

bool draw_dragger(gui_window *win, const char *lbl, float &value,
                  float speed = 1.0f, float min = 0.0f, float max = 0.0f);

bool draw_dragger(gui_window *win, const char *lbl, vec2f &value,
                  float speed = 1.0f, float min = 0.0f, float max = 0.0f);

bool draw_dragger(gui_window *win, const char *lbl, vec3f &value,
                  float speed = 1.0f, float min = 0.0f, float max = 0.0f);

bool draw_dragger(gui_window *win, const char *lbl, vec4f &value,
                  float speed = 1.0f, float min = 0.0f, float max = 0.0f);

bool draw_dragger(gui_window *win, const char *lbl, int &value, float speed = 1,
                  int min = 0, int max = 0);

bool draw_dragger(gui_window *win, const char *lbl, vec2i &value,
                  float speed = 1, int min = 0, int max = 0);

bool draw_dragger(gui_window *win, const char *lbl, vec3i &value,
                  float speed = 1, int min = 0, int max = 0);

bool draw_dragger(gui_window *win, const char *lbl, vec4i &value,
                  float speed = 1, int min = 0, int max = 0);

bool draw_checkbox(gui_window *win, const char *lbl, bool &value);

bool draw_checkbox(gui_window *win, const char *lbl, bool &value, bool invert);

bool draw_coloredit(gui_window *win, const char *lbl, vec3f &value);

bool draw_coloredit(gui_window *win, const char *lbl, vec4f &value);

bool draw_hdrcoloredit(gui_window *win, const char *lbl, vec3f &value);

bool draw_hdrcoloredit(gui_window *win, const char *lbl, vec4f &value);

bool draw_combobox(gui_window *win, const char *lbl, int &idx, const std::vector<std::string> &labels);

bool draw_combobox(gui_window *win, const char *lbl, std::string &value, const std::vector<std::string> &labels);

bool draw_combobox(gui_window *win, const char *lbl, int &idx, int num, const std::function<std::string(int)> &labels,
                   bool include_null = false);

template<typename T>
inline bool draw_combobox(gui_window *win, const char *lbl, T *&value,
                          const std::vector<T *> &vals, bool include_null = false) {
    auto idx = -1;
    for (auto pos = 0; pos < vals.size(); pos++)
        if (vals[pos] == value) idx = pos;
    auto edited = draw_combobox(
            win, lbl, idx, (int) vals.size(), [&](int idx) { return vals[idx]->name; },
            include_null);
    if (edited) {
        value = idx >= 0 ? vals[idx] : nullptr;
    }
    return edited;
}

template<typename T>
inline bool draw_combobox(gui_window *win, const char *lbl, T *&value,
                          const std::vector<T *> &vals, const std::vector<std::string> &labels,
                          bool include_null = false) {
    auto idx = -1;
    for (auto pos = 0; pos < vals.size(); pos++)
        if (vals[pos] == value) idx = pos;
    auto edited = draw_combobox(
            win, lbl, idx, (int) vals.size(), [&](int idx) { return labels[idx]; },
            include_null);
    if (edited) {
        value = idx >= 0 ? vals[idx] : nullptr;
    }
    return edited;
}

void draw_progressbar(gui_window *win, const char *lbl, float fraction);

void draw_progressbar(gui_window *win, const char *lbl, int current, int total);

void draw_histogram(
        gui_window *win, const char *lbl, const std::vector<float> &values);

void draw_histogram(
        gui_window *win, const char *lbl, const std::vector<vec2f> &values);

void draw_histogram(
        gui_window *win, const char *lbl, const std::vector<vec3f> &values);

void draw_histogram(
        gui_window *win, const char *lbl, const std::vector<vec4f> &values);

bool draw_filedialog(gui_window *win, const char *lbl, std::string &path, bool save,
                     const std::string &dirname, const std::string &filename, const std::string &filter);

bool draw_filedialog_button(gui_window *win, const char *button_lbl,
                            bool button_active, const char *lbl, std::string &path, bool save,
                            const std::string &dirname, const std::string &filename, const std::string &filter);

void log_info(gui_window *win, const std::string &msg);

void log_error(gui_window *win, const std::string &msg);

void clear_log(gui_window *win);

void draw_log(gui_window *win);

}  // namespace bcg

#endif
