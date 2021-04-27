//
// Created by alex on 14.04.21.
//

#include "bcg_gui_mesh_connected_components.h"
#include "viewer/bcg_viewer_state.h"
#include "mesh/bcg_mesh_connected_components.h"

namespace bcg{

void gui_mesh_connected_components(viewer_state *state){
    static int num_connected_components = -1;
    if(ImGui::Button("Split Connected Components")){
        if(state->scene.valid(state->picker.entity_id)){
            if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
                auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                num_connected_components = static_cast<int>(mesh_connected_components_detect(mesh));
                state->dispatcher.trigger<event::mesh::connected_components::split>(state->picker.entity_id);
            }
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Detect Connected Components")){
        if(state->scene.valid(state->picker.entity_id)){
            if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
                auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                num_connected_components = static_cast<int>(mesh_connected_components_detect(mesh));
            }
        }
    }
    if(num_connected_components > 0){
        ImGui::LabelText("num_connected_components", "%d", num_connected_components);
    }
}

}