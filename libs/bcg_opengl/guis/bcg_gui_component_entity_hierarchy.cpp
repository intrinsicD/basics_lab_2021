//
// Created by alex on 19.11.20.
//

#include "bcg_gui_component_entity_hierarchy.h"
#include "bcg_gui_entity_components.h"
#include "components/bcg_component_entity_hierarchy.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_show_transform.h"

namespace bcg{

void gui_component_entity_hierarchy(viewer_state *state){
    gui_component_entity_hierarchy(state, state->picker.entity_id);
}

void gui_component_entity_hierarchy(viewer_state *state, entt::entity id){
    if(!state->scene.valid(id)) return;
    if(!state->scene.has<entity_hierarchy>(id)) return;
    auto &hierarchy = state->scene.get<entity_hierarchy>(id);
    ImGui::Text("Accumulated Transform");
    gui_show_transform(hierarchy.accumulated_model);
    ImGui::Separator();
    if(hierarchy.parent != entt::null){
        ImGui::LabelText("Parent", "%zu", size_t(hierarchy.parent));
    }else{
        ImGui::LabelText("Parent", "%s", "-");
    }
    if(ImGui::CollapsingHeader("Children")){
        for(const auto child_entity : hierarchy.children){
            auto child_id = child_entity.second;
            if (ImGui::TreeNode(("Child" + std::to_string(int(child_id))).c_str())) {
                gui_entity_components(state, child_id);
                ImGui::TreePop();
            }
        }
    }
}

}