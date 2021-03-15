//
// Created by alex on 17.11.20.
//

#include "bcg_gui_viewer_window.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg{

void gui_viewer_window(viewer_state *state){
    if (ImGui::CollapsingHeader("Window")) {
        draw_label(&state->window, "title", state->window.title);
        draw_label(&state->window, "width", std::to_string(state->window.width));
        draw_label(&state->window, "height", std::to_string(state->window.height));
        std::stringstream ss;
        ss << state->window.framebuffer_viewport.transpose();
        draw_label(&state->window, "framebuffer_viewport", ss.str());
        ImGui::InputInt("widget_width", &state->window.widgets_width, 1, 10);
        ImGui::Separator();
    }
}

}