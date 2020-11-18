//
// Created by alex on 17.11.20.
//


#include "bcg_gui_mesh_vertex_normals.h"
#include "bcg_viewer_state.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"

namespace bcg{

void gui_mesh_vertex_normals(viewer_state *state){
    static std::vector<std::string> names = {"uniform", "angle", "area", "area_angle"};
    static int current_value = 0;
    draw_combobox(&state->window, "type", current_value, names);
    if(ImGui::Button("compute vertex normals")){
        using types = event::mesh::vertex_normals::Type;
        switch (static_cast<types>(current_value)){
            case types::UNIFORM : {
                state->dispatcher.trigger<event::mesh::vertex_normals::uniform>(state->picker.entity_id);
                break;
            }
            case types::ANGLE : {
                state->dispatcher.trigger<event::mesh::vertex_normals::angle>(state->picker.entity_id);
                break;
            }
            case types::AREA : {
                state->dispatcher.trigger<event::mesh::vertex_normals::area>(state->picker.entity_id);
                break;
            }
            case types::AREAANGLE : {
                state->dispatcher.trigger<event::mesh::vertex_normals::area_angle>(state->picker.entity_id);
                break;
            }
        }
        auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
        auto &color = material.attributes[1];
        color.property_name = "v_normal";
        state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
    }
}

}