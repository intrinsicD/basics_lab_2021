//
// Created by alex on 19.11.20.
//
#include <string>
#include <vector>
#include "bcg_gui_mesh_subdivision.h"
#include "bcg_viewer_state.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"

namespace bcg {

void gui_mesh_subdivision(viewer_state *state){
    static std::vector<std::string> names = {"catmull_clark", "loop", "sqrt3"};
    static int current_method = 0;
    draw_combobox(&state->window, "method", current_method, names);
    if(ImGui::Button("Compute")){
        if("catmull_clark" == names[current_method]){
            state->dispatcher.trigger<event::mesh::subdivision::catmull_clark>(state->picker.entity_id);
        }else if("loop" == names[current_method]){
            state->dispatcher.trigger<event::mesh::subdivision::loop>(state->picker.entity_id);
        }else if("sqrt3" == names[current_method]){
            state->dispatcher.trigger<event::mesh::subdivision::sqrt3>(state->picker.entity_id);
        }
        auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
        state->dispatcher.trigger<event::gpu::update_vertex_attributes>(state->picker.entity_id, material.attributes);
        auto edge_attributes = {attribute{"edges", "edges", "edges", 0, true}};
        state->dispatcher.trigger<event::gpu::update_edge_attributes>(state->picker.entity_id, edge_attributes);
        auto face_attributes = {attribute{"triangles", "triangles", "triangles", 0, true}};
        state->dispatcher.trigger<event::gpu::update_face_attributes>(state->picker.entity_id, face_attributes);
    }
}

}