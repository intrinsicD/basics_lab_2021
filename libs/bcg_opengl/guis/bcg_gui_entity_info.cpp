//
// Created by alex on 16.11.20.
//

#include "bcg_gui_entity_info.h"
#include "bcg_viewer_state.h"
#include "bcg_entity_info.h"
#include "utils/bcg_path.h"
#include "bcg_imgui.h"

namespace bcg{

void gui_entity_info(viewer_state *state, entity_info *info, entt::entity id){
    if (!info) return;
    if (ImGui::CollapsingHeader("info")) {
        draw_label(&state->window, "entity_id", std::to_string((unsigned int) id));
        draw_label(&state->window, "filename", path_filename(info->filename));
        draw_textinput(&state->window, "entity_name", info->entity_name);
        std::stringstream ss;
        ss << info->loading_center.transpose();
        draw_label(&state->window, "loading_center", ss.str());
        draw_label(&state->window, "loading_scale", std::to_string(info->loading_scale));
        ImGui::Separator();
    }
}

}