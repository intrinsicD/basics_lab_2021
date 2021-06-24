//
// Created by alex on 16.11.20.
//

#include "bcg_gui_viewer_state.h"
#include "bcg_gui_viewer_colors.h"
#include "bcg_gui_viewer_window.h"
#include "bcg_gui_viewer_keyboard.h"
#include "bcg_gui_viewer_mouse.h"
#include "bcg_gui_viewer_time.h"
#include "bcg_gui_viewer_gui.h"
#include "bcg_gui_viewer_picker.h"
#include "bcg_gui_viewer_camera.h"
#include "viewer/bcg_imgui.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_component_entity_info.h"
#include "bcg_gui_show_aligned_box3.h"
#include "bcg_gui_ogl_shape.h"
#include "bcg_gui_entity_components.h"

namespace bcg {

void gui_viewer_state(viewer_state *state) {
    if (ImGui::CollapsingHeader("Scene")) {
        auto view = state->scene().view<entity_info>();
        for (const auto id : view) {
            auto &info = state->scene.get<entity_info>(id);
            std::stringstream ss;
            ss << info.entity_name << " id: " << std::to_string((unsigned int) id);
            if (ImGui::TreeNode(ss.str().c_str())) {
                gui_entity_components(state, id);
                ImGui::TreePop();
            }
        }
    }

    gui_viewer_colors(state);
    gui_viewer_window(state);
    gui_viewer_keyboard(state);
    gui_viewer_mouse(state);
    gui_viewer_time(state);
    gui_viewer_gui(state);
    gui_viewer_picker(state);
    gui_viewer_camera(state);
}

}