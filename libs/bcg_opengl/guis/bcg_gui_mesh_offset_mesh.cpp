//
// Created by alex on 13.04.21.
//

#include "bcg_gui_mesh_offset_mesh.h"
#include "viewer/bcg_viewer_state.h"
#include "mesh/bcg_mesh_offset_mesh.h"

namespace bcg{

void gui_mesh_offset_mesh(viewer_state *state){
    static float offset = 0;
    static float sigma_p = 0;

    ImGui::InputFloat("Offset", &offset);
    ImGui::InputFloat("Sigma_p", &sigma_p);
    if(state->scene.valid(state->picker.entity_id)){
        if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
            if(ImGui::Button("Apply offset")){
                auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                mesh_normal_offset_mesh(mesh, offset);
            }
            ImGui::Separator();
            if(ImGui::Button("Compute offset mesh and resolve")){
                auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                mesh_face_normal_offset_mesh(mesh, offset, sigma_p, state->config.parallel_grain_size);
            }
        }
    }
}

}