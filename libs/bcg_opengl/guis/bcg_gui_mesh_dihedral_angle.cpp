//
// Created by alex on 16.11.20.
//

#include "bcg_gui_mesh_dihedral_angle.h"
#include "bcg_viewer_state.h"

namespace bcg{

void gui_mesh_dihedral_angle(viewer_state *state){
    if(ImGui::Button("Compute Dihedral Angles")){
        state->dispatcher.trigger<event::mesh::edge::dihedral_angle>(state->picker.entity_id);
    }
}

}