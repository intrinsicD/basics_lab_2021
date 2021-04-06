//
// Created by alex on 06.04.21.
//

#include "bcg_gui_mesh_laplacian_harmonic_field.h"
#include "bcg_gui_mesh_laplacian.h"
#include "viewer/bcg_viewer_state.h"
#include "viewer/bcg_selection.h"
#include "mesh/bcg_mesh_boundary.h"
#include "math/laplacian/bcg_laplacian_harmonic_field.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"

namespace bcg{

void gui_mesh_laplacian_harmonic_field(viewer_state *state){
    gui_mesh_laplacian(state);
    static bool vertex_selection_mode = state->picker.mode == viewer_picker::Mode::vertices;
    if(ImGui::Checkbox("vertex_selection_mode", &vertex_selection_mode)){
        if(vertex_selection_mode){
            state->dispatcher.trigger<event::picker::enable::vertex>();
        }else{
            state->dispatcher.trigger<event::picker::disable>();
        }
        vertex_selection_mode = state->picker.mode == viewer_picker::Mode::vertices;
    }
    static float vertex_values = 0;
    static float weight = 1000;
    ImGui::InputFloat("vertex_values", &vertex_values);
    ImGui::InputFloat("weight", &weight);
    if(ImGui::Button("Set Boundary Values")){
        if(state->scene.valid(state->picker.entity_id)){
            if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
                auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                auto v_constrained_values = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_constrained_values");
                auto v_feature = mesh.vertices.get_or_add<bool, 1>("v_feature");
                auto boundary = mesh_boundary(mesh, state->config.parallel_grain_size);
                for(const auto &v : boundary.boundary_vertices){
                    v_constrained_values[v] = vertex_values;
                    v_feature[v] = true;
                }

                auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
                auto &color = material.attributes[2];
                color.property_name = "v_constrained_values";
                state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
            }
        }
    }
    if(ImGui::Button("Set Vertex Values")){
        if(state->scene.valid(state->picker.entity_id)){
            if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
                auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                auto v_constrained_values = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_constrained_values");
                auto v_feature = mesh.vertices.get_or_add<bool, 1>("v_feature");
                if(state->scene.all_of<selected_vertices>(state->picker.entity_id)){
                    auto &selection = state->scene.get<selected_vertices>(state->picker.entity_id);
                    for(const auto &item : selection.selected){
                        v_feature[item.second.element] = true;
                        v_constrained_values[item.second.element] = vertex_values;
                    }

                    auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
                    auto &color = material.attributes[2];
                    color.property_name = "v_constrained_values";
                    state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
                }
            }
        }
    }
    if(ImGui::Button("Clear Vertex Values")){
        if(state->scene.valid(state->picker.entity_id)){
            if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
                auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                auto v_constrained_values = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_constrained_values");
                v_constrained_values.reset(0);
                auto v_feature = mesh.vertices.get_or_add<bool, 1>("v_feature");
                v_feature.reset(false);
            }
        }
    }
    if(ImGui::Button("Compute Harmonic Field")){
        if(state->scene.valid(state->picker.entity_id)){
            if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
                auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                auto v_constrained_values = mesh.vertices.get_or_add<bcg_scalar_t, 1>("v_constrained_values");

                if(state->scene.all_of<mesh_laplacian>(state->picker.entity_id)){
                    auto &laplacian = state->scene.get<mesh_laplacian>(state->picker.entity_id);
                    laplacian_harmonic_field(&mesh.vertices, laplacian, v_constrained_values, weight);
                    auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
                    auto &color = material.attributes[2];
                    color.property_name = "v_harmonic_field";
                    state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
                }
            }
        }
    }

}

}