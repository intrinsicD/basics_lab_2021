//
// Created by alex on 16.11.20.
//

#include "bcg_gui_mesh_dihedral_angle.h"
#include "bcg_viewer_state.h"
#include "renderers/graph_renderer/bcg_material_graph.h"
#include "renderers/graph_renderer/bcg_events_graph_renderer.h"

namespace bcg{

void gui_mesh_dihedral_angle(viewer_state *state){
    if(ImGui::Button("Compute Dihedral Angles")){
        state->dispatcher.trigger<event::mesh::edge::dihedral_angle>(state->picker.entity_id);
        auto &material = state->scene.get_or_emplace<material_graph>(state->picker.entity_id);
        auto &color = material.attributes[1];
        color.property_name = "e_dihedral_angle";
        state->dispatcher.trigger<event::graph_renderer::set_color_texture>(state->picker.entity_id, color);
    }
}

}