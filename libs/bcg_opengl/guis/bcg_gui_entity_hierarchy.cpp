//
// Created by alex on 19.11.20.
//

#include "components/bcg_component_entity_hierarchy.h"
#include "viewer/bcg_viewer_state.h"
#include "components/bcg_component_entity_info.h"
#include "bcg_gui_entity_info.h"

namespace bcg{

void gui_entity_hierarchy(viewer_state *state, entity_hierarchy *hierarchy){
    if(!hierarchy) return;
    if(ImGui::CollapsingHeader("Children")){
        for(const auto child : hierarchy->children){
            auto child_id = child.second;
            if (ImGui::TreeNode(("child" + std::to_string(int(child_id))).c_str())) {
                gui_entity_info(state, state->scene.try_get<entity_info>(child_id), child_id);
                gui_entity_hierarchy(state, state->scene.try_get<entity_hierarchy>(child_id));
                ImGui::TreePop();
            }
        }
    }
}

}