//
// Created by alex on 14.01.21.
//

#include "bcg_gui_mesh_smoothing.h"
#include "bcg_gui_property_selector.h"
#include "bcg_gui_reload_entity.h"
#include "bcg_gui_mesh_laplacian.h"
#include "viewer/bcg_viewer_state.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"

namespace bcg {

void gui_mesh_smoothing(viewer_state *state) {
    static std::string current_property_name;
    bool show_property = gui_property_selector(state,
                                               state->get_vertices(state->picker.entity_id),
                                               {1, 3},
                                               "properties",
                                               current_property_name);
    static int e = 0;
    ImGui::RadioButton("implicit", &e, 0);
    ImGui::RadioButton("explicit", &e, 1);
    ImGui::RadioButton("taubin", &e, 2);
    static float timestep = 0.5;
    static float lambda = 0.33;
    static float mu = -0.34;
    static int smoothing_steps = 1;

    if(e == 0 || e == 1){
        ImGui::InputFloat("timestep", &timestep);
    }else if (e == 1) {
        ImGui::InputInt("smoothing_steps", &smoothing_steps);
    }else if(e == 2){
        ImGui::InputFloat("lambda", &lambda,0,0,"%.12f");
        ImGui::InputFloat("mu", &mu, 0,0,"%.12f");
        ImGui::InputInt("smoothing_steps", &smoothing_steps);
    }
    gui_mesh_laplacian(state);
    if (ImGui::Button("Compute") && state->scene.valid(state->picker.entity_id)) {
        auto id = state->picker.entity_id;
        auto *vertices = state->get_vertices(id);
        if (vertices) {
            auto &material = state->scene.get<material_mesh>(id);
            auto &color = material.attributes[2];
            color.property_name = current_property_name;

            auto *base_ptr = vertices->get_base_ptr(current_property_name);
            if (base_ptr && e != 3) {
                if (e == 0) {
                    if (base_ptr->dims() == 1) {
                        auto property = vertices->get<bcg_scalar_t, 1>(current_property_name);
                        if (property) {
                            state->dispatcher.trigger<event::mesh::smoothing::implicit_smoothing_1D>(id, timestep,
                                                                                                     property);
                            show_property = true;
                        }
                    } else if (base_ptr->dims() == 3) {
                        auto property = vertices->get<VectorS<3>, 3>(current_property_name);
                        if (property) {
                            state->dispatcher.trigger<event::mesh::smoothing::implicit_smoothing_3D>(id, timestep,
                                                                                                     property);
                            show_property = true;
                        }
                    }
                } else if (e == 1) {
                    if (base_ptr->dims() == 1) {
                        auto property = vertices->get<bcg_scalar_t, 1>(current_property_name);
                        if (property) {
                            state->dispatcher.trigger<event::mesh::smoothing::explicit_smoothing_1D>(id,
                                                                                                     smoothing_steps,
                                                                                                     timestep,
                                                                                                     property);
                            show_property = true;
                        }
                    } else if (base_ptr->dims() == 3) {
                        auto property = vertices->get<VectorS<3>, 3>(current_property_name);
                        if (property) {
                            state->dispatcher.trigger<event::mesh::smoothing::explicit_smoothing_3D>(id,
                                                                                                     smoothing_steps,
                                                                                                     timestep,
                                                                                                     property);
                            show_property = true;
                        }
                    }
                }
            } else {
                //Smooth geometry
                if (e == 0) {
                    state->dispatcher.trigger<event::mesh::smoothing::implicit_smoothing>(id, timestep);
                } else if (e == 1) {
                    state->dispatcher.trigger<event::mesh::smoothing::explicit_smoothing>(id, smoothing_steps,
                                                                                          timestep);
                }else if (e == 2){
                    state->dispatcher.trigger<event::mesh::smoothing::taubin_smoothing>(id, lambda, mu, smoothing_steps);
                }
            }
        }
    }
    if (show_property && state->scene.valid(state->picker.entity_id)) {
        auto id = state->picker.entity_id;
        auto &material = state->scene.get<material_mesh>(id);
        auto &color = material.attributes[2];
        color.property_name = current_property_name;
        state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(id, color);
    }
    gui_reload_entity(state);
}

}