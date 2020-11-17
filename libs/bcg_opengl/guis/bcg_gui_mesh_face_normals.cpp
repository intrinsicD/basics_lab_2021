//dihedral_angle
// Created by alex on 16.11.20.
//


#include "bcg_gui_mesh_face_normals.h"
#include "bcg_viewer_state.h"

namespace bcg{

void gui_mesh_face_normals(viewer_state *state){
    if(ImGui::Button("Compute Face Normals")){
        state->dispatcher.trigger<event::mesh::face::normals>(state->picker.entity_id);
    }
}

}