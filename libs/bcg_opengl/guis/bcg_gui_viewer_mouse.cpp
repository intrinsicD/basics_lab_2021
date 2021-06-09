//
// Created by alex on 17.11.20.
//

#include "bcg_gui_viewer_mouse.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg{

void gui_viewer_mouse(viewer_state *state){
    if (ImGui::CollapsingHeader("Mouse")) {
        ImGui::Checkbox("moving", &state->mouse.is_moving);
        ImGui::Checkbox("scrolling", &state->mouse.is_scrolling);
        ImGui::Checkbox("dragging", &state->mouse.is_dragging);
        ImGui::Checkbox("captured by gui", &state->gui.captured_mouse);
        ImGui::Checkbox("left pressed", &state->mouse.left);
        ImGui::Checkbox("middle pressed", &state->mouse.middle);
        ImGui::Checkbox("right pressed", &state->mouse.right);
        ImGui::Separator();
        std::stringstream ss;
        ss << state->mouse.last_left_click.transpose();
        draw_label(&state->window, "last left click", ss.str());
        ss.str("");
        ss << state->mouse.last_middle_click.transpose();
        draw_label(&state->window, "last middle click", ss.str());
        ss.str("");
        ss << state->mouse.last_right_click.transpose();
        draw_label(&state->window, "last right click", ss.str());
        ss.str("");
        ss << state->mouse.cursor_position.transpose();
        draw_label(&state->window, "cursor_position", ss.str());
        ss.str("");
        ss << state->mouse.normalized_device_coordinates.transpose();
        draw_label(&state->window, "normalized_device_coords", ss.str());
        ss.str("");
        ss << state->mouse.last_cursor_position.transpose();
        draw_label(&state->window, "last_cursor_position", ss.str());
        ss.str("");
        ss << state->mouse.cursor_delta.transpose();
        draw_label(&state->window, "cursor_delta", ss.str());
        draw_label(&state->window, "scroll_value", std::to_string(state->mouse.scroll_value));
        ImGui::Separator();
    }
}

}