//
// Created by alex on 29.03.21.
//

#include "bcg_gui_mesh_vertex_extremal_values.h"
#include "bcg_gui_property_selector.h"
#include "viewer/bcg_viewer_state.h"
#include "mesh/bcg_mesh_vertex_extremal_values.h"
#include "bcg_opengl/renderers/mesh_renderer/bcg_material_mesh.h"
#include "bcg_opengl/renderers/mesh_renderer/bcg_events_mesh_renderer.h"

namespace bcg{

void gui_mesh_vertex_extremal_values(viewer_state *state){
    static int e = 0;
    static std::string current_property_name;
    if(state->scene.valid(state->picker.entity_id) && state->scene.has<halfedge_mesh>(state->picker.entity_id)){
        auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
        gui_property_selector(state, &mesh.vertices, {1}, "property", current_property_name);
        draw_combobox(&state->window, "type", e, mesh_vertex_extremal_type_names());

        if(ImGui::Button("Filter Values")){
            if(!current_property_name.empty()){
                auto property = mesh.vertices.get<bcg_scalar_t, 1>(current_property_name);
                std::string property_name;
                switch (static_cast<MeshVertexExtremalType>(e)){
                    case MeshVertexExtremalType::minimum : {
                        mesh_vertex_minimal_values(mesh, property, state->config.parallel_grain_size);
                        property_name = "v_minimal_values";
                        break;
                    }

                    case MeshVertexExtremalType::maximum : {
                        mesh_vertex_maximal_values(mesh, property, state->config.parallel_grain_size);
                        property_name = "v_maximal_values";
                        break;
                    }

                    case MeshVertexExtremalType::extremal : {
                        mesh_vertex_extremal_values(mesh, property, state->config.parallel_grain_size);
                        property_name = "v_extremal_values";
                        break;
                    }

                    case MeshVertexExtremalType::__last__ : {
                        break;
                    }
                }
                if(!property_name.empty()){
                    auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
                    auto &color = material.attributes[2];
                    color.property_name = property_name;
                    state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
                }
            }
        }
    }
}

}