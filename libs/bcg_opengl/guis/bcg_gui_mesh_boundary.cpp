//
// Created by alex on 19.11.20.
//

#include "bcg_gui_mesh_boundary.h"
#include "bcg_viewer_state.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"

namespace bcg {

void gui_mesh_boundary(viewer_state *state){
    if(ImGui::Button("Compute Boundary")){
        state->dispatcher.trigger<event::mesh::boundary>(state->picker.entity_id);
        auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
        auto &color = material.attributes[2];
        color.property_name = "v_boundary";
        state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
    }
}

}
