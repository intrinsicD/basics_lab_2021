//
// Created by alex on 16.11.20.
//

#include "bcg_gui_component_entity_info.h"
#include "viewer/bcg_viewer_state.h"
#include "components/bcg_component_entity_info.h"
#include "utils/bcg_path.h"

namespace bcg{

void gui_component_entity_info(viewer_state *state){
    gui_component_entity_info(state, state->picker.entity_id);
}

void gui_component_entity_info(viewer_state *state, entt::entity id){
    if(!state->scene.has<entity_info>(id)) return;
    auto &info = state->scene.get<entity_info>(id);
    ImGui::LabelText("Entity ID", "%zu", size_t(id));
    ImGui::LabelText("Filename", "%s", path_filename(info.filename).c_str());
    draw_textinput(&state->window, "entity_name", info.entity_name);
}

}