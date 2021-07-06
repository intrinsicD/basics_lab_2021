//
// Created by alex on 24.06.21.
//

#include "bcg_gui_entity_components.h"
#include "viewer/bcg_viewer_state.h"
#include "components/bcg_component_entity_hierarchy.h"
#include "bcg_gui_component_entity_info.h"
#include "bcg_gui_component_entity_hierarchy.h"
#include "bcg_gui_component_loading_backup.h"
#include "bcg_gui_component_transform_world_space.h"
#include "bcg_gui_components_geometry.h"
#include "bcg_gui_rendering_options.h"
#include "bcg_gui_show_aligned_box3.h"

namespace bcg{

void gui_entity_components(viewer_state *state){
    gui_entity_components(state, state->picker.entity_id);
}

void gui_entity_components(viewer_state *state, entt::entity id){
    if(!state->scene.valid(id)) return;
    if(ImGui::CollapsingHeader("Info")){
        gui_component_entity_info(state, id);
        ImGui::Separator();
    }
    if(ImGui::CollapsingHeader("Aligned Box")){
        gui_show_aligned_box3(state->scene.try_get<aligned_box3>(id));
        ImGui::Separator();
    }
    if(ImGui::CollapsingHeader("World Space Transform")){
        gui_component_transform_world_space(state, id);
        ImGui::Separator();
    }
    if(ImGui::CollapsingHeader("Loading Backup")){
        gui_component_loading_backup(state, id);
        ImGui::Separator();
    }
    if(state->scene.has<entity_hierarchy>(id) && ImGui::CollapsingHeader("Hierarchy")){
        gui_component_entity_hierarchy(state, id);
        ImGui::Separator();
    }
    if(ImGui::CollapsingHeader("Rendering Options")){
        gui_rendering_options(state, id);
        ImGui::Separator();
    }
    if(ImGui::CollapsingHeader("Geometry")){
        gui_components_geometry(state, id);
    }
    ImGui::Separator();
}

}