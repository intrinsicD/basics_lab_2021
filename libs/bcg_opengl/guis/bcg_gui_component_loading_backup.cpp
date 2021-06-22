//
// Created by alex on 22.06.21.
//

#include "bcg_gui_component_loading_backup.h"
#include "viewer/bcg_viewer_state.h"
#include "components/bcg_component_loading_backup.h"
#include "bcg_gui_show_transform.h"
#include "bcg_gui_show_aligned_box3.h"

namespace bcg {

void gui_component_loading_backup(viewer_state *state) {
    gui_component_loading_backup(state, state->picker.entity_id);
}

void gui_component_loading_backup(viewer_state *state, entt::entity id) {
    if (!state->scene.valid(id)) return;
    if (!state->scene.has<loading_backup>(id)) return;
    auto &backup = state->scene.get<loading_backup>(id);
    ImGui::Text("World Space Transform");
    ImGui::Separator();
    gui_show_transform(backup.ws_model);
    ImGui::Separator();
    ImGui::Text("Object Space Transform");
    ImGui::Separator();
    gui_show_transform(backup.os_model);
    ImGui::Separator();
    ImGui::Text("Aligned Box");
    ImGui::Separator();
    gui_show_aligned_box3(&backup.aabb);
    ImGui::Separator();
}


}