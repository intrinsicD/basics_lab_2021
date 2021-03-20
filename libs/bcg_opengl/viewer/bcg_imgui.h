#ifndef BCG_GRAPHICS_BCG_IMGUI
#define BCG_GRAPHICS_BCG_IMGUI

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

#include "imgui/imgui.h"
#include "bcg_library/math/bcg_linalg.h"

namespace bcg {

struct viewer_state;
struct viewer_window;

struct gui_element {
    gui_element();

    virtual ~gui_element() = default;

    std::function<void(viewer_state *state)> show;

    virtual void render(viewer_state *state);

    bool active;
};

struct gui_menu {
    std::function<bool(viewer_state *state, gui_menu *self)> show;

    void render(viewer_state *state);
};

struct left_panel : public gui_element {
    using gui_element::gui_element;

    left_panel &operator=(const std::function<void(viewer_state *state)> &cb);

    void render(viewer_state *state) override;
};

bool begin_header(viewer_window *win, const char *title);

void end_header(viewer_window *win);

void draw_label(viewer_window *win, const char *lbl, const std::string &text);

void draw_separator(viewer_window *win);

void continue_line(viewer_window *win);

bool draw_button(viewer_window *win, const char *lbl, bool enabled = true);

bool draw_textinput(viewer_window *win, const char *lbl, std::string &value);

template<typename T>
bool draw_input(viewer_window *, const char *lbl, T &value){
    float vf = value;
    auto edited = ImGui::InputFloat(lbl, &vf);
    value = vf;
    return edited;
}

template<typename T>
bool draw_input_vec2(viewer_window *, const char *lbl, Vector<T, 2> &value){
    Vector<float, 2> vf = value.template cast<float>();
    bool edited = ImGui::InputFloat2(lbl, vf.data());
    value = vf.cast<T>();
    return edited;
}

template<typename T>
bool draw_input_vec3(viewer_window *, const char *lbl, Vector<T, 3> &value){
    Vector<float, 3> vf = value.template cast<float>();
    bool edited = ImGui::InputFloat3(lbl, vf.data());
    value = vf.cast<T>();
    return edited;
}

template<typename T>
bool draw_input_vec4(viewer_window *, const char *lbl, Vector<T, 4> &value){
    Vector<float, 4> vf = value.template cast<float>();
    bool edited = ImGui::InputFloat4(lbl, vf.data());
    value = vf.cast<T>();
    return edited;
}

bool draw_slider(
        viewer_window *win, const char *lbl, float &value, float min, float max);

bool draw_slider(
        viewer_window *win, const char *lbl, Vector<float, 2> &value, float min, float max);

bool draw_slider(
        viewer_window *win, const char *lbl, Vector<float, 3> &value, float min, float max);

bool draw_slider(
        viewer_window *win, const char *lbl, Vector<float, 4> &value, float min, float max);

bool draw_slider(
        viewer_window *win, const char *lbl, int &value, int min, int max);

bool draw_slider(
        viewer_window *win, const char *lbl, VectorI<2> &value, int min, int max);

bool draw_slider(
        viewer_window *win, const char *lbl, VectorI<3> &value, int min, int max);

bool draw_slider(
        viewer_window *win, const char *lbl, VectorI<4> &value, int min, int max);

bool draw_dragger(viewer_window *win, const char *lbl, float &value,
                  float speed = 1.0f, float min = 0.0f, float max = 0.0f);

bool draw_dragger(viewer_window *win, const char *lbl, Vector<float, 2> &value,
                  float speed = 1.0f, float min = 0.0f, float max = 0.0f);

bool draw_dragger(viewer_window *win, const char *lbl, Vector<float, 3> &value,
                  float speed = 1.0f, float min = 0.0f, float max = 0.0f);

bool draw_dragger(viewer_window *win, const char *lbl, Vector<float, 4> &value,
                  float speed = 1.0f, float min = 0.0f, float max = 0.0f);

bool draw_dragger(viewer_window *win, const char *lbl, int &value, float speed = 1,
                  int min = 0, int max = 0);

bool draw_dragger(viewer_window *win, const char *lbl, VectorI<2> &value,
                  float speed = 1, int min = 0, int max = 0);

bool draw_dragger(viewer_window *win, const char *lbl, VectorI<3> &value,
                  float speed = 1, int min = 0, int max = 0);

bool draw_dragger(viewer_window *win, const char *lbl, VectorI<4> &value,
                  float speed = 1, int min = 0, int max = 0);

bool draw_checkbox(viewer_window *win, const char *lbl, bool &value);

bool draw_checkbox(viewer_window *win, const char *lbl, bool &value, bool invert);

bool draw_coloredit(viewer_window *win, const char *lbl, Vector<float, 3> &value);

bool draw_coloredit(viewer_window *win, const char *lbl, Vector<float, 4> &value);

bool draw_hdrcoloredit(viewer_window *win, const char *lbl, Vector<float, 3> &value);

bool draw_hdrcoloredit(viewer_window *win, const char *lbl, Vector<float, 4> &value);

bool draw_combobox(viewer_window *win, const char *lbl, int &idx, const std::vector<std::string> &labels);

bool draw_combobox(viewer_window *win, const char *lbl, std::string &value, const std::vector<std::string> &labels);

bool
draw_combobox(viewer_window *win, const char *lbl, int &idx, int num, const std::function<std::string(int)> &labels,
              bool include_null = false);

template<typename T>
inline bool draw_combobox(viewer_window *win, const char *lbl, T *&value,
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
inline bool draw_combobox(viewer_window *win, const char *lbl, T *&value,
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

void draw_progressbar(viewer_window *win, const char *lbl, float fraction);

void draw_progressbar(viewer_window *win, const char *lbl, int current, int total);

void draw_histogram(
        viewer_window *win, const char *lbl, const std::vector<float> &values);

void draw_histogram(
        viewer_window *win, const char *lbl, const std::vector<Vector<float, 2>>

&values);

void draw_histogram(
        viewer_window *win, const char *lbl, const std::vector<Vector<float, 3>>

&values);

void draw_histogram(
        viewer_window *win, const char *lbl, const std::vector<Vector<float, 4>>

&values);

bool draw_filedialog(viewer_window *win, const char *lbl, std::string &path, bool save,
                     const std::string &dirname, const std::string &filename, const std::string &filter);

bool draw_filedialog_button(viewer_window *win, const char *button_lbl,
                            bool button_active, const char *lbl, std::string &path, bool save,
                            const std::string &dirname, const std::string &filename, const std::string &filter);

void log_info(viewer_window *win, const std::string &msg);

void log_error(viewer_window *win, const std::string &msg);

void clear_log(viewer_window *win);

void draw_log(viewer_window *win);

}  // namespace bcg

#endif
