//
// Created by alex on 13.04.21.
//

#include "bcg_gui_mesh_offset_mesh.h"
#include "viewer/bcg_viewer_state.h"
#include "mesh/bcg_mesh_offset_mesh.h"

namespace bcg{

void gui_mesh_offset_mesh(viewer_state *state){
    static float offset = 0;

    ImGui::InputFloat("Offset", &offset);
    if(state->scene.valid(state->picker.entity_id)){
        if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
            if(ImGui::Button("Apply offset")){
                auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                mesh_offset_mesh(mesh, offset);
            }
        }
    }
}

}