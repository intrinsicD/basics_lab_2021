//dihedral_angle
// Created by alex on 16.11.20.
//


#include "bcg_gui_mesh_face_normals.h"
#include "viewer/bcg_viewer_state.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"

namespace bcg{

void gui_mesh_face_normals(viewer_state *state){
    if(ImGui::Button("compute face normals")){
        state->dispatcher.trigger<event::mesh::face::normals>(state->picker.entity_id);
        auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
        auto &color = material.attributes[1];
        color.property_name = "f_normal";
        state->dispatcher.trigger<event::mesh_renderer::set_face_color_attribute>(state->picker.entity_id, color);
    }
}

}