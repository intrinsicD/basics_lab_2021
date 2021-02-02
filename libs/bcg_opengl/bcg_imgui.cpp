#include <mutex>

#include "bcg_imgui.h"
#include "bcg_viewer_state.h"
#include "bcg_library/utils/bcg_path.h"

#include <GLFW/glfw3.h>
#include "imgui/imgui_internal.h"

#include "bcg_entity_info.h"
#include "aligned_box/bcg_aligned_box.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"
#include "renderers/graph_renderer/bcg_material_graph.h"
#include "renderers/vectorfield_renderer/bcg_material_vectorfield.h"
#include "renderers/points_renderer/bcg_material_points.h"
#include "renderers/curve_renderer/bcg_material_curve.h"
#include "renderers/points_renderer/bcg_events_points_renderer.h"
#include "renderers/graph_renderer/bcg_events_graph_renderer.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "renderers/vectorfield_renderer/bcg_events_vectorfield_renderer.h"
#include "renderers/curve_renderer/bcg_events_curve_renderer.h"
#include "renderers/bcg_vectorfields.h"
#include "geometry/mesh/bcg_mesh.h"
#include "utils/bcg_path.h"

namespace bcg {
using namespace std::string_literals;

gui_element::gui_element() : show(nullptr), active(false) {

}

void gui_element::render(viewer_state *state) {
    if (show) show(state);
}

void gui_menu::render(viewer_state *state) {
    if (show) {
        ImGui::BeginMainMenuBar();
        state->gui.menu_height = (int) ImGui::GetWindowSize()[1];
        show(state, this);
        ImGui::EndMainMenuBar();
    }
}

left_panel &left_panel::operator=(const std::function<void(viewer_state *)> &cb) {
    active = true;
    show = cb;
    return *this;
}

void left_panel::render(viewer_state *state) {
    if (!show) return;
    int window[2];
    glfwGetWindowSize(state->window.win, &window[0], &window[1]);

    if (state->gui.menu.show) {
        ImGui::SetNextWindowPos({0, (float) state->gui.menu_height});
    } else {
        ImGui::SetNextWindowPos({0, 0});
    }
    ImGui::SetNextWindowSize({(float) state->window.widgets_width, (float) (window[1] - state->gui.menu_height)});

    ImGui::SetNextWindowBgAlpha(1);

    if (ImGui::Begin(" ", &active,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
        show(state);
    }
    ImGui::End();
}

bool begin_header(viewer_window *win, const char *lbl) {
    if (!ImGui::CollapsingHeader(lbl)) return false;
    ImGui::PushID(lbl);
    return true;
}

void end_header(viewer_window *win) { ImGui::PopID(); }

void open_glmodal(viewer_window *win, const char *lbl) { ImGui::OpenPopup(lbl); }

void clear_glmodal(viewer_window *win) { ImGui::CloseCurrentPopup(); }

bool begin_glmodal(viewer_window *win, const char *lbl) {
    return ImGui::BeginPopupModal(lbl);
}

void end_glmodal(viewer_window *win) { ImGui::EndPopup(); }

bool is_glmodal_open(viewer_window *win, const char *lbl) {
    return ImGui::IsPopupOpen(lbl);
}

struct filedialog_state {
    std::string dirname = "";
    std::string filename = "";
    std::vector<std::pair<std::string, bool>> entries = {};
    bool save = false;
    bool remove_hidden = true;
    std::string filter = "";
    std::vector<std::string> extensions = {};

    filedialog_state() = default;

    filedialog_state(const std::string &dirname, const std::string &filename,
                     const std::string &filter, bool save) {
        set(dirname, filename, filter, save);
    }

    void set(const std::string &dirname, const std::string &filename, const std::string &filter,
             bool save) {
        this->save = save;
        _set_filter(filter);
        _set_dirname(dirname);
        _set_filename(filename);
    }

    void _set_dirname(const std::string &name) {
        if (path_exists(name) && path_isdir(name)) {
            dirname = name;
        } else if (path_exists(dirname) && path_isdir(dirname)) {
            // leave it like this
        } else {
            dirname = path_current();
        }
        dirname = normalize_path(dirname);
        entries.clear();
        for (auto entry : list_directory(dirname)) {
            if (remove_hidden && path_basename(entry)[0] == '.') continue;
            if (path_isdir(entry)) {
                entries.push_back({path_filename(entry) + "/", true});
            } else {
                entries.push_back({path_filename(entry), false});
            }
        }
        std::sort(entries.begin(), entries.end(), [](auto &a, auto &b) {
            if (a.second == b.second) return a.first < b.first;
            return a.second;
        });
    }

    void _set_filename(const std::string &name) {
        filename = name;
        if (filename.empty()) return;
        auto ext = path_extension(filename);
        if (std::find(extensions.begin(), extensions.end(), ext) ==
            extensions.end()) {
            filename = "";
            return;
        }
        if (!save && !path_exists(path_join(dirname, filename))) {
            filename = "";
            return;
        }
    }

    void _set_filter(const std::string &flt) {
        auto globs = std::vector<std::string>{""};
        for (auto i = 0; i < flt.size(); i++) {
            if (flt[i] == ';') {
                globs.push_back("");
            } else {
                globs.back() += flt[i];
            }
        }
        filter = "";
        extensions.clear();
        for (auto pattern : globs) {
            if (pattern == "") continue;
            auto ext = path_extension(pattern);
            if (ext != "") {
                extensions.push_back(ext);
                filter += (filter == "") ? ("*." + ext) : (";*." + ext);
            }
        }
    }

    void select(int idx) {
        if (entries[idx].second) {
            set(path_join(dirname, entries[idx].first), filename, filter, save);
        } else {
            set(dirname, entries[idx].first, filter, save);
        }
    }

    std::string get_path() const { return path_join(dirname, filename); }
};

bool draw_filedialog(viewer_window *win, const char *lbl, std::string &path, bool save,
                     const std::string &dirname, const std::string &filename, const std::string &filter) {
    static auto states = std::unordered_map<std::string, filedialog_state>{};
    ImGui::SetNextWindowSize({500, 300}, ImGuiCond_FirstUseEver);
    if (ImGui::BeginPopupModal(lbl)) {
        if (states.find(lbl) == states.end()) {
            states[lbl] = filedialog_state{dirname, filename, filter, save};
        }
        auto &state = states.at(lbl);
        auto dir_buffer = std::array<char,
                1024>{};
        snprintf(dir_buffer.data(), dir_buffer.size(), "%s", state.dirname.c_str());
        if (ImGui::InputText("dir", dir_buffer.data(), sizeof(dir_buffer))) {
            state.set(dir_buffer.data(), state.filename, state.filter, save);
        }
        auto current_item = -1;
        if (ImGui::ListBox(
                "entries", &current_item,
                [](void *data, int idx, const char **out_text) -> bool {
                    auto &state = *(filedialog_state *) data;
                    *out_text = state.entries[idx].first.c_str();
                    return true;
                },
                &state, (int) state.entries.size())) {
            state.select(current_item);
        }
        auto file_buffer = std::array<char,
                1024>{};
        snprintf(
                file_buffer.data(), file_buffer.size(), "%s", state.filename.c_str());
        if (ImGui::InputText("file", file_buffer.data(), file_buffer.size())) {
            state.set(state.dirname, file_buffer.data(), state.filter, save);
        }
        auto filter_buffer = std::array<char,
                1024>{};
        snprintf(
                filter_buffer.data(), filter_buffer.size(), "%s", state.filter.c_str());
        if (ImGui::InputText(
                "filter", filter_buffer.data(), filter_buffer.size())) {
            state.set(state.dirname, state.filename, filter_buffer.data(), save);
        }
        auto ok = false, exit = false;
        if (ImGui::Button("Ok")) {
            path = state.dirname + state.filename;
            ok = true;
            exit = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            exit = true;
        }
        if (exit) {
            ImGui::CloseCurrentPopup();
            states.erase(lbl);
        }
        ImGui::EndPopup();
        return ok;
    } else {
        return false;
    }
}

bool draw_filedialog_button(viewer_window *win, const char *button_lbl,
                            bool button_active, const char *lbl, std::string &path, bool save,
                            const std::string &dirname, const std::string &filename, const std::string &filter) {
    if (is_glmodal_open(win, lbl)) {
        draw_button(win, button_lbl, button_active);
        return draw_filedialog(win, lbl, path, save, dirname, filename, filter);
    } else {
        if (draw_button(win, button_lbl, button_active)) {
            open_glmodal(win, lbl);
        }
        return false;
    }
}

bool draw_button(viewer_window *win, const char *lbl, bool enabled) {
    if (enabled) {
        return ImGui::Button(lbl);
    } else {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        auto ok = ImGui::Button(lbl);
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
        return ok;
    }
}

void draw_label(viewer_window *win, const char *lbl, const std::string &label) {
    ImGui::LabelText(lbl, "%s", label.c_str());
}

void draw_separator(viewer_window *win) { ImGui::Separator(); }

void continue_line(viewer_window *win) { ImGui::SameLine(); }

bool draw_textinput(viewer_window *win, const char *lbl, std::string &value) {
    auto buffer = std::array<char,
            4096>{};
    auto num = 0;
    for (auto c : value) buffer[num++] = c;
    buffer[num] = 0;
    auto edited = ImGui::InputText(lbl, buffer.data(), buffer.size());
    if (edited) value = buffer.data();
    return edited;
}

bool draw_slider(
        viewer_window *win, const char *lbl, float &value, float min, float max) {
    return ImGui::SliderFloat(lbl, &value, min, max);
}

bool draw_slider(
        viewer_window *win, const char *lbl, VectorS<2> &value, float min, float max) {
    return ImGui::SliderFloat2(lbl, reinterpret_cast<float *>(value.data()), min, max);
}

bool draw_slider(
        viewer_window *win, const char *lbl, VectorS<3> &value, float min, float max) {
    return ImGui::SliderFloat3(lbl, reinterpret_cast<float *>(value.data()), min, max);
}

bool draw_slider(
        viewer_window *win, const char *lbl, VectorS<4> &value, float min, float max) {
    return ImGui::SliderFloat4(lbl, reinterpret_cast<float *>(value.data()), min, max);
}

bool draw_slider(
        viewer_window *win, const char *lbl, int &value, int min, int max) {
    return ImGui::SliderInt(lbl, &value, min, max);
}

bool draw_slider(
        viewer_window *win, const char *lbl, VectorI<2> &value, int min, int max) {
    return ImGui::SliderInt2(lbl, reinterpret_cast<int *>(value.data()), min, max);
}

bool draw_slider(
        viewer_window *win, const char *lbl, VectorI<3> &value, int min, int max) {
    return ImGui::SliderInt3(lbl, reinterpret_cast<int *>(value.data()), min, max);
}

bool draw_slider(
        viewer_window *win, const char *lbl, VectorI<4> &value, int min, int max) {
    return ImGui::SliderInt4(lbl, reinterpret_cast<int *>(value.data()), min, max);
}

bool draw_dragger(viewer_window *win, const char *lbl, float &value, float speed,
                  float min, float max) {
    return ImGui::DragFloat(lbl, &value, speed, min, max);
}

bool draw_dragger(viewer_window *win, const char *lbl, VectorS<2> &value, float speed,
                  float min, float max) {
    return ImGui::DragFloat2(lbl, reinterpret_cast<float *>(value.data()), speed, min, max);
}

bool draw_dragger(viewer_window *win, const char *lbl, VectorS<3> &value, float speed,
                  float min, float max) {
    return ImGui::DragFloat3(lbl, reinterpret_cast<float *>(value.data()), speed, min, max);
}

bool draw_dragger(viewer_window *win, const char *lbl, VectorS<4> &value, float speed,
                  float min, float max) {
    return ImGui::DragFloat4(lbl, reinterpret_cast<float *>(value.data()), speed, min, max);
}

bool draw_dragger(viewer_window *win, const char *lbl, int &value, float speed,
                  int min, int max) {
    return ImGui::DragInt(lbl, &value, speed, min, max);
}

bool draw_dragger(viewer_window *win, const char *lbl, VectorI<2> &value, float speed,
                  int min, int max) {
    return ImGui::DragInt2(lbl, reinterpret_cast<int *>(value.data()), speed, min, max);
}

bool draw_dragger(viewer_window *win, const char *lbl, VectorI<3> &value, float speed,
                  int min, int max) {
    return ImGui::DragInt3(lbl, reinterpret_cast<int *>(value.data()), speed, min, max);
}

bool draw_dragger(viewer_window *win, const char *lbl, VectorI<4> &value, float speed,
                  int min, int max) {
    return ImGui::DragInt4(lbl, reinterpret_cast<int *>(value.data()), speed, min, max);
}

bool draw_checkbox(viewer_window *win, const char *lbl, bool &value) {
    return ImGui::Checkbox(lbl, &value);
}

bool draw_checkbox(viewer_window *win, const char *lbl, bool &value, bool invert) {
    if (!invert) {
        return draw_checkbox(win, lbl, value);
    } else {
        auto inverted = !value;
        auto edited = ImGui::Checkbox(lbl, &inverted);
        if (edited) value = !inverted;
        return edited;
    }
}

bool draw_coloredit(viewer_window *win, const char *lbl, VectorS<3> &value) {
    auto flags = ImGuiColorEditFlags_Float;
    Vector<float, 3> color = value.cast<float>();
    auto edit = ImGui::ColorEdit3(lbl, color.data(), flags);
    if (edit) {
        value = color.cast<bcg_scalar_t>();
        return true;
    }
    return false;
}

bool draw_coloredit(viewer_window *win, const char *lbl, VectorS<4> &value) {
    auto flags = ImGuiColorEditFlags_Float;
    Vector<float, 4> color = value.cast<float>();
    auto edit = ImGui::ColorEdit4(lbl, color.data(), flags);
    if (edit) {
        value = color.cast<bcg_scalar_t>();
        return true;
    }
    return false;
}

bool draw_hdrcoloredit(viewer_window *win, const char *lbl, VectorS<3> &value) {
    auto color = value;
    auto exposure = 0.0f;
    auto scale = color.maxCoeff();
    if (scale > 1) {
        color /= scale;
        exposure = log2(scale);
    }
    auto edit_exposure = draw_slider(
            win, (lbl + " [exp]"s).c_str(), exposure, 0, 10);
    auto edit_color = draw_coloredit(win, (lbl + " [col]"s).c_str(), color);
    if (edit_exposure || edit_color) {
        value = color * exp2(exposure);
        return true;
    } else {
        return false;
    }
}

bool draw_hdrcoloredit(viewer_window *win, const char *lbl, VectorS<4> &value) {
    auto color = value;
    auto exposure = 0.0f;
    auto scale = color.head<3>().maxCoeff();
    if (scale > 1) {
        color[0] /= scale;
        color[1] /= scale;
        color[2] /= scale;
        exposure = log2(scale);
    }
    auto edit_exposure = draw_slider(
            win, (lbl + " [exp]"s).c_str(), exposure, 0, 10);
    auto edit_color = draw_coloredit(win, (lbl + " [col]"s).c_str(), color);
    if (edit_exposure || edit_color) {
        value[0] = color[0] * exp2(exposure);
        value[1] = color[1] * exp2(exposure);
        value[2] = color[2] * exp2(exposure);
        value[3] = color[3];
        return true;
    } else {
        return false;
    }
}

bool draw_combobox(viewer_window *win, const char *lbl, int &value,
                   const std::vector<std::string> &labels) {
    if(value >= labels.size()) return false;
    if (!ImGui::BeginCombo(lbl, labels[value].c_str())) return false;
    auto old_val = value;
    for (auto i = 0; i < labels.size(); i++) {
        ImGui::PushID(i);
        if (ImGui::Selectable(labels[i].c_str(), value == i)) value = i;
        if (value == i) ImGui::SetItemDefaultFocus();
        ImGui::PopID();
    }
    ImGui::EndCombo();
    return value != old_val;
}

bool draw_combobox(viewer_window *win, const char *lbl, std::string &value,
                   const std::vector<std::string> &labels) {
    if (!ImGui::BeginCombo(lbl, value.c_str())) return false;
    auto old_val = value;
    for (auto i = 0; i < labels.size(); i++) {
        ImGui::PushID(i);
        if (ImGui::Selectable(labels[i].c_str(), value == labels[i]))
            value = labels[i];
        if (value == labels[i]) ImGui::SetItemDefaultFocus();
        ImGui::PopID();
    }
    ImGui::EndCombo();
    return value != old_val;
}

bool draw_combobox(viewer_window *win, const char *lbl, int &idx, int num,
                   const std::function<std::string(int)> &labels, bool include_null) {
    if (num <= 0) idx = -1;
    if (!ImGui::BeginCombo(lbl, idx >= 0 ? labels(idx).c_str() : "<none>"))
        return false;
    auto old_idx = idx;
    if (include_null) {
        ImGui::PushID(100000);
        if (ImGui::Selectable("<none>", idx < 0)) idx = -1;
        if (idx < 0) ImGui::SetItemDefaultFocus();
        ImGui::PopID();
    }
    for (auto i = 0; i < num; i++) {
        ImGui::PushID(i);
        if (ImGui::Selectable(labels(i).c_str(), idx == i)) idx = i;
        if (idx == i) ImGui::SetItemDefaultFocus();
        ImGui::PopID();
    }
    ImGui::EndCombo();
    return idx != old_idx;
}

void draw_progressbar(viewer_window *win, const char *lbl, float fraction) {
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.5, 0.5, 1, 0.25));
    ImGui::ProgressBar(fraction, ImVec2(0.0f, 0.0f));
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text(lbl, ImVec2(0.0f, 0.0f));
    ImGui::PopStyleColor(1);
}

void draw_progressbar(
        viewer_window *win, const char *lbl, int current, int total) {
    auto overlay = std::to_string(current) + "/" + std::to_string(total);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.5, 0.5, 1, 0.25));
    ImGui::ProgressBar(
            (float) current / (float) total, ImVec2(0.0f, 0.0f), overlay.c_str());
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text(lbl, ImVec2(0.0f, 0.0f));
    ImGui::PopStyleColor(1);
}

void draw_histogram(
        viewer_window *win, const char *lbl, const float *values, int count) {
    ImGui::PlotHistogram(lbl, values, count);
}

void draw_histogram(
        viewer_window *win, const char *lbl, const std::vector<float> &values) {
    ImGui::PlotHistogram(lbl, values.data(), (int) values.size(), 0, nullptr,
                         scalar_max, scalar_max, {0, 0}, 4);
}

void draw_histogram(
        viewer_window *win, const char *lbl, const std::vector<VectorS<2>> &values) {
    ImGui::PlotHistogram((lbl + " x"s).c_str(), (const float *) values.data() + 0,
                         (int) values.size(), 0, nullptr, scalar_max, scalar_max, {0, 0}, sizeof(VectorS<2>));
    ImGui::PlotHistogram((lbl + " y"s).c_str(), (const float *) values.data() + 1,
                         (int) values.size(), 0, nullptr, scalar_max, scalar_max, {0, 0}, sizeof(VectorS<2>));
}

void draw_histogram(
        viewer_window *win, const char *lbl, const std::vector<VectorS<3>> &values) {
    ImGui::PlotHistogram((lbl + " x"s).c_str(), (const float *) values.data() + 0,
                         (int) values.size(), 0, nullptr, scalar_max, scalar_max, {0, 0}, sizeof(VectorS<3>));
    ImGui::PlotHistogram((lbl + " y"s).c_str(), (const float *) values.data() + 1,
                         (int) values.size(), 0, nullptr, scalar_max, scalar_max, {0, 0}, sizeof(VectorS<3>));
    ImGui::PlotHistogram((lbl + " z"s).c_str(), (const float *) values.data() + 2,
                         (int) values.size(), 0, nullptr, scalar_max, scalar_max, {0, 0}, sizeof(VectorS<3>));
}

void draw_histogram(
        viewer_window *win, const char *lbl, const std::vector<VectorS<4>> &values) {
    ImGui::PlotHistogram((lbl + " x"s).c_str(), (const float *) values.data() + 0,
                         (int) values.size(), 0, nullptr, scalar_max, scalar_max, {0, 0}, sizeof(VectorS<4>));
    ImGui::PlotHistogram((lbl + " y"s).c_str(), (const float *) values.data() + 1,
                         (int) values.size(), 0, nullptr, scalar_max, scalar_max, {0, 0}, sizeof(VectorS<4>));
    ImGui::PlotHistogram((lbl + " z"s).c_str(), (const float *) values.data() + 2,
                         (int) values.size(), 0, nullptr, scalar_max, scalar_max, {0, 0}, sizeof(VectorS<4>));
    ImGui::PlotHistogram((lbl + " w"s).c_str(), (const float *) values.data() + 3,
                         (int) values.size(), 0, nullptr, scalar_max, scalar_max, {0, 0}, sizeof(VectorS<4>));
}

// https://github.com/ocornut/imgui/issues/300
struct ImGuiAppLog {
    ImGuiTextBuffer Buf;
    ImGuiTextFilter Filter;
    ImVector<int> LineOffsets;  // Index to lines offset
    bool ScrollToBottom;

    void Clear() {
        Buf.clear();
        LineOffsets.clear();
    }

    void AddLog(const char *msg, const char *lbl) {
        auto old_size = Buf.size();
        Buf.appendf("[%s] %s\n", lbl, msg);
        for (auto new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n') LineOffsets.push_back(old_size);
        ScrollToBottom = true;
    }

    void Draw() {
        if (ImGui::Button("Clear")) Clear();
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        Filter.Draw("Filter", -100.0f);
        ImGui::Separator();
        ImGui::BeginChild("scrolling");
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
        if (copy) ImGui::LogToClipboard();

        if (Filter.IsActive()) {
            const char *buf_begin = Buf.begin();
            const char *line = buf_begin;
            for (int line_no = 0; line != nullptr; line_no++) {
                const char *line_end = (line_no < LineOffsets.Size)
                                       ? buf_begin + LineOffsets[line_no]
                                       : nullptr;
                if (Filter.PassFilter(line, line_end))
                    ImGui::TextUnformatted(line, line_end);
                line = line_end != nullptr && line_end[1] != 0 ? line_end + 1 : nullptr;
            }
        } else {
            ImGui::TextUnformatted(Buf.begin());
        }

        if (ScrollToBottom) ImGui::SetScrollHere(1.0f);
        ScrollToBottom = false;
        ImGui::PopStyleVar();
        ImGui::EndChild();
    }

    void Draw(const char *title, bool *p_opened = nullptr) {
        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin(title, p_opened);
        Draw();
        ImGui::End();
    }
};

std::mutex _log_mutex;
ImGuiAppLog _log_widget;

void log_info(viewer_window *win, const std::string &msg) {
    _log_mutex.lock();
    _log_widget.AddLog(msg.c_str(), "info");
    _log_mutex.unlock();
}

void log_error(viewer_window *win, const std::string &msg) {
    _log_mutex.lock();
    _log_widget.AddLog(msg.c_str(), "errn");
    _log_mutex.unlock();
}

void clear_log(viewer_window *win) {
    _log_mutex.lock();
    _log_widget.Clear();
    _log_mutex.unlock();
}

void draw_log(viewer_window *win) {
    _log_mutex.lock();
    _log_widget.Draw();
    _log_mutex.unlock();
}

}  // namespace bcg
