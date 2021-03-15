//
// Created by alex on 19.11.20.
//

#include "bcg_gui_mesh_vertex_convex_concave.h"
#include "viewer/bcg_viewer_state.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"

namespace bcg {

void gui_mesh_vertex_convex_concave(viewer_state *state){
    if(ImGui::Button("compute vertex convex concave")){
        state->dispatcher.trigger<event::mesh::vertex_convex_concave>(state->picker.entity_id);
        auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
        auto &color = material.attributes[2];
        color.property_name = "v_convex";
        state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
    }
}

}