//
// Created by alex on 16.11.20.
//

#include "bcg_gui_entity_info.h"
#include "viewer/bcg_viewer_state.h"
#include "components/bcg_component_entity_info.h"
#include "utils/bcg_path.h"
#include "viewer/bcg_imgui.h"

namespace bcg{

void gui_entity_info(viewer_state *state, entity_info *info, entt::entity id){
    if (!info) return;
    if (ImGui::CollapsingHeader("info")) {
        draw_label(&state->window, "entity_id", std::to_string((unsigned int) id));
        draw_label(&state->window, "filename", path_filename(info->filename));
        draw_textinput(&state->window, "entity_name", info->entity_name);
        std::stringstream ss;
        ss << info->loading_model.matrix();
        draw_label(&state->window, "loading_model", ss.str());
        ss.str() = "";
        ss << info->loading_aabb;
        draw_label(&state->window, "loading_aabb", ss.str());
        ImGui::Separator();
    }
}

}