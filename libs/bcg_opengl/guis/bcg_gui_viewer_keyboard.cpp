//
// Created by alex on 17.11.20.
//

#include "bcg_gui_viewer_keyboard.h"
#include "bcg_viewer_state.h"

namespace bcg{

void gui_viewer_keyboard(viewer_state *state){
    if (ImGui::CollapsingHeader("Keyboard")) {
        ImGui::Checkbox("shift pressed", &state->keyboard.shift_pressed);
        ImGui::Checkbox("alt pressed", &state->keyboard.alt_pressed);
        ImGui::Checkbox("ctrl pressed", &state->keyboard.ctrl_pressed);
        ImGui::Checkbox("command pressed", &state->keyboard.command_pressed);
        ImGui::Checkbox("no modifier", &state->keyboard.no_modifier);
        ImGui::Checkbox("captured by gui", &state->keyboard.is_captured_by_gui);
        ImGui::Separator();
    }
}

}