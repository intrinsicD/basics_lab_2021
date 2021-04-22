//
// Created by alex on 08.04.21.
//

#include "bcg_gui_segmented_jaw_alignment.h"
#include "bcg_gui_graph_dijkstra.h"
#include "bcg_gui_property_selector.h"
#include "bcg_gui_property_classify_max.h"
#include "bcg_gui_property_clamp.h"
#include "bcg_property_utils.h"
#include "bcg_property_map_eigen.h"
#include "graph/bcg_graph_edge_from_vertex_function.h"
#include "bcg_gui_mesh_laplacian_harmonic_field.h"
#include "viewer/bcg_viewer_state.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "renderers/points_renderer/bcg_material_points.h"
#include "renderers/points_renderer/bcg_events_points_renderer.h"
#include "math/rotations/bcg_rotation_chordal_mean.h"

namespace bcg{

void gui_segmented_jaw_alignment(viewer_state *state){
    if(state->scene.valid(state->picker.entity_id)){
        if(ImGui::CollapsingHeader("Compute Segmentation")){
            static bool compute_edge_scaling = false;
            ImGui::Checkbox("Compute Edge Scaling", &compute_edge_scaling);
            static std::string computed_property_name;
            static std::string result_property_name;
            if(compute_edge_scaling){
                static int post_smoothing_steps = 3;
                static bool two_ring_neighborhood = true;
                ImGui::InputInt("post_smoothing_steps", &post_smoothing_steps);
                ImGui::Checkbox("two_ring_neighborhood", &two_ring_neighborhood);
                bool triggered_compute = false;
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
                if(ImGui::Button("Invert Function")){
                    if(state->scene.all_of<halfedge_mesh>(state->picker.entity_id)){
                        auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                        auto vertex_property = mesh.vertices.get<bcg_scalar_t, 1>(computed_property_name);
                        auto result = invert(&mesh.vertices, vertex_property);
                        triggered_compute = true;
                        computed_property_name = result.name();
                    }
                }
                if(triggered_compute){
                    auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
                    auto &color = material.attributes[2];
                    color.property_name = computed_property_name;
                    state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
                }

                if(!computed_property_name.empty()) {
                    auto result = gui_property_classify_max(state);
                    result_property_name = result.current_property_name;
                    if (!result_property_name.empty()) {
                        ImGui::Separator();
                        static float value_small = 0.1;
                        static float value_large = 1000;
                        ImGui::InputFloat("value_small", &value_small);
                        ImGui::InputFloat("value_large", &value_large);

                        auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
                        if (ImGui::Button("Compute Edge Scaling Function")) {
                            auto vertex_property = mesh.vertices.get<bcg_scalar_t, 1>(result_property_name);
                            auto vertex_property_original = mesh.vertices.get<bcg_scalar_t, 1>(computed_property_name);
                            if (vertex_property) {
                                edge_from_vertex_boolean_function_or(mesh, "e_laplacian_scaling", vertex_property,
                                                                     value_small,
                                                                     value_large, state->config.parallel_grain_size);
                                edge_from_vertex_function_min(mesh, "e_dijkstra_scaling", vertex_property_original, state->config.parallel_grain_size);
                                auto e_dijkstra_scaling = mesh.edges.get<bcg_scalar_t, 1>("e_dijkstra_scaling");
                                if(e_dijkstra_scaling){
                                    auto result_shift = shift(&mesh.edges, e_dijkstra_scaling, -MapConst(e_dijkstra_scaling).minCoeff());
                                    Map(e_dijkstra_scaling) = MapConst(result_shift);
                                    Map(e_dijkstra_scaling).array() /= MapConst(e_dijkstra_scaling).maxCoeff();
                                }
                            } else {
                                std::cout << "property " << result_property_name << " does not exist!\n";
                            }
                        }
                    }
                }
            }


            if(ImGui::CollapsingHeader("Harmonic Segmentation")){
                if(compute_edge_scaling){
                    if(!computed_property_name.empty()){
                        if(!result_property_name.empty()){
                            auto &mesh = state->scene.get<halfedge_mesh>(state->picker.entity_id);
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
                if(compute_edge_scaling){
                    gui_graph_dijkstra(state);
                }
            }
        }
        struct Tooth{
            Tooth(entt::entity id, bool selected) : entity_id(id), selected(selected) {}
            entt::entity entity_id;
            bool selected = true;
        };
        static std::vector<Tooth> source_teeth;
        static entt::entity source_jaw, source, target;
        static bool hide_jaw = false;
        if(ImGui::CollapsingHeader("Compute Alignment")){
            if(ImGui::Button("Set selected as Source")){
                if(state->scene.valid(state->picker.entity_id)){
                    source = state->picker.entity_id;
                }
            }
            if(ImGui::Button("Set selected as Target")){
                if(state->scene.valid(state->picker.entity_id)){
                    target = state->picker.entity_id;
                }
            }
            if(ImGui::Button("Set selected as Source Jaw")){
                if(state->scene.valid(state->picker.entity_id)){
                    source_jaw = state->picker.entity_id;
                }
            }
            if(ImGui::Button("Add selection to teeth")){
                source_teeth.clear();
                for(const auto &item : state->picker.selected_entities){
                    source_teeth.emplace_back(item.second, true);
                }
            }
            if(ImGui::Button("Clear Teeth")){
                source_teeth.clear();
            }
            if(state->scene.valid(source)){
                ImGui::LabelText("source ", "%d", int(source));
            }
            if(state->scene.valid(source_jaw)){
                ImGui::LabelText("Jaw", "%d", int(source_jaw));
                ImGui::SameLine();
                if(ImGui::Checkbox("Hide Jaw", &hide_jaw)){
                    if(hide_jaw){
                        state->scene.remove<event::mesh_renderer::enqueue>(source_jaw);
                    }else{
                        state->scene.emplace<event::mesh_renderer::enqueue>(source_jaw);
                    }
                }
            }
            if(state->scene.valid(target)){
                ImGui::LabelText("Target ", "%d", int(target));
            }

            for(auto &tooth: source_teeth){
                ImGui::Checkbox(("tooth " + std::to_string(int(tooth.entity_id))).c_str(), &tooth.selected);
            }
            static int selected_registration_method;
            static auto registration_methods = registration_names();
            registration_methods.resize(2);
            draw_combobox(&state->window, "Registration Method", selected_registration_method, registration_methods);

            if(ImGui::Button("Compute Alignment Step")){
                for(const auto &tooth : source_teeth){
                    state->dispatcher.trigger<event::registration::align_step>(tooth.entity_id, target, static_cast<RegistrationMethod>(selected_registration_method));
                }
                chordal_mean_so3 mean(true, true);
                std::vector<MatrixS<3, 3>> rotations;
                VectorS<3> position = VectorS<3>::Zero();
                int count = 0;
                for(const auto &tooth : source_teeth){
                    if(tooth.selected){
                        auto model = state->scene.get<Transform>(tooth.entity_id);
                        rotations.emplace_back(model.linear());
                        position += model.translation();
                        ++count;
                    }
                }
                position /= bcg_scalar_t(count);
                MatrixS<3, 3> result = mean(rotations);
                auto &source_model = state->scene.get<Transform>(source);
                source_model.linear() = result;
                source_model.translation() = position;
            }
        }
    }
}

}