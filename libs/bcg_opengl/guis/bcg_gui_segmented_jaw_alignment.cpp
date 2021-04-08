//
// Created by alex on 08.04.21.
//

#include "bcg_gui_segmented_jaw_alignment.h"
#include "bcg_gui_graph_dijkstra.h"
#include "bcg_gui_property_selector.h"
#include "bcg_gui_property_classify_max.h"
#include "bcg_gui_property_clamp.h"
#include "bcg_property_map_eigen.h"
#include "graph/bcg_graph_edge_from_vertex_function.h"
#include "bcg_gui_mesh_laplacian_harmonic_field.h"
#include "viewer/bcg_viewer_state.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "renderers/points_renderer/bcg_material_points.h"
#include "renderers/points_renderer/bcg_events_points_renderer.h"

namespace bcg{

void gui_segmented_jaw_alignment(viewer_state *state){
    if(state->scene.valid(state->picker.entity_id)){
        if(ImGui::CollapsingHeader("Compute Segmentation")){
            if(ImGui::CollapsingHeader("Harmonic Segmentation")){
                static bool compute_edge_scaling = false;
                ImGui::Checkbox("Compute Edge Scaling", &compute_edge_scaling);
                if(compute_edge_scaling){
                    static int post_smoothing_steps = 3;
                    static bool two_ring_neighborhood = true;
                    ImGui::InputInt("post_smoothing_steps", &post_smoothing_steps);
                    ImGui::Checkbox("two_ring_neighborhood", &two_ring_neighborhood);
                    bool triggered_compute = false;
                    static std::string computed_property_name;
                    if (ImGui::Button("Compute Vertex Convex Concave")) {
                        state->dispatcher.trigger<event::mesh::vertex_convex_concave>(state->picker.entity_id, post_smoothing_steps,
                                                                                      two_ring_neighborhood);
                        triggered_compute = true;
                        computed_property_name = "v_concave";
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Compute Vertex Curvatures")) {
                        triggered_compute = true;
                        state->dispatcher.trigger<event::mesh::curvature::taubin>(state->picker.entity_id, post_smoothing_steps, two_ring_neighborhood);
                        computed_property_name = "v_mesh_curv_mean";
                    }
                    if(triggered_compute){
                        auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
                        auto &color = material.attributes[2];
                        color.property_name = computed_property_name;
                        state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
                    }
                    if(!computed_property_name.empty()){
                        auto result = gui_property_classify_max(state);
                        if(!result.current_property_name.empty()){
                            ImGui::Separator();
                            static float value_small = 0.1;
                            static float value_large = 1000;
                            ImGui::InputFloat("value_small", &value_small);
                            ImGui::InputFloat("value_large", &value_large);

                            auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                            if (ImGui::Button("Compute Edge Scaling Function")) {
                                std::cout << "property " << result.current_property_name << " does exist!\n";
                                auto vertex_property = mesh.vertices.get<bcg_scalar_t, 1>(result.current_property_name);
                                if (vertex_property) {
                                    edge_from_vertex_boolean_function_or(mesh, "e_laplacian_scaling", vertex_property,
                                                                         value_small,
                                                                         value_large, state->config.parallel_grain_size);
                                }else{
                                    std::cout << "property " << result.current_property_name << " does not exist!\n";
                                }
                            }
                            if(mesh.edges.has("e_laplacian_scaling")){
                                ImGui::Separator();
                                gui_mesh_laplacian_harmonic_field(state);
                                if(state->picker.mode == viewer_picker::Mode::vertices && state->scene.all_of<material_points>(state->picker.entity_id)){
                                    auto &material = state->scene.get<material_points>(state->picker.entity_id);
                                    if(material.attributes[2].buffer_name.empty() && mesh.vertices.has("v_selected")){
                                        material.attributes[2].property_name = "v_selected";
                                        state->dispatcher.trigger<event::points_renderer::set_point_size_attribute>(state->picker.entity_id, material.attributes[2]);
                                    }
                                }
                                if(mesh.vertices.has("v_harmonic_field")){
                                    ImGui::Separator();
                                    auto v_harmon_field = mesh.vertices.get<bcg_scalar_t, 1>("v_harmonic_field");
                                    static float hf_clamp_min = -0.5;
                                    static float hf_clamp_max = 0.5;
                                    gui_property_clamp(state, &mesh.vertices, v_harmon_field, hf_clamp_min, hf_clamp_max);
                                }
                            }
                        }

                    }
                }
            }
            if(ImGui::CollapsingHeader("Dijkstra Segmentation")){
                gui_graph_dijkstra(state);
            }
        }
        if(ImGui::CollapsingHeader("Compute Alignment")){

        }
    }
}

}