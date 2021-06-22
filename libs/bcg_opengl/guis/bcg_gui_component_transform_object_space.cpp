//
// Created by alex on 22.06.21.
//

#include "bcg_gui_component_transform_object_space.h"
#include "bcg_gui_edit_transform.h"
#include "components/bcg_component_transform_object_space.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg{

void gui_component_transform_object_space(viewer_state *state){
    gui_component_transform_object_space(state, state->picker.entity_id);
}

void gui_component_transform_object_space(viewer_state *state, entt::entity id){
    if(!state->scene.valid(id)) return;
    if(!state->scene.has<object_space_transform>(id)) return;
    Transform &osm = state->scene.get<object_space_transform>(id);
    gui_edit_transform(state, osm);
    ImGui::Separator();
    static bool button_scene_scaling_used = false;
    if(!button_scene_scaling_used && ImGui::Button("Remove Scene Scaling")){
        osm = osm * state->scene.scaling.inverse();
        button_scene_scaling_used = true;
    }

    if(button_scene_scaling_used && ImGui::Button("Apply Scene Scaling")){
        osm = osm * state->scene.scaling;
        button_scene_scaling_used = false;
    }
    ImGui::SameLine();
    if(ImGui::Button("Set Identity")){
        osm.setIdentity();
    }
}

}