//
// Created by alex on 18.04.21.
//

#include "bcg_gui_mesh_vertex_quadrics.h"
#include "bcg_gui_property_selector.h"
#include "bcg_gui_point_cloud_vertex_noise.h"
#include "mesh/bcg_mesh_vertex_quadric.h"
#include "mesh/bcg_mesh_vertex_normals.h"
#include "viewer/bcg_viewer_state.h"
#include "utils/bcg_string_utils.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"

namespace bcg {

void gui_mesh_vertex_quadrics(viewer_state *state) {
    static auto type_names = mesh_vertex_quadric_type_names();
    static int selected_type = 0;
    draw_combobox(&state->window, "quadric_type", selected_type, type_names);

    static VectorS<3> anisotropic_sigma_p = VectorS<3>::Ones() * 0.01;
    static VectorS<3> anisotropic_sigma_n = VectorS<3>::Ones() * 0.01;
    static float isotropic_sigma_p = 0.01;
    static float isotropic_sigma_n = 0.01;

    static std::string selected_local_sigma_p;
    static std::string selected_local_sigma_n;
    static std::string selected_normals;
    static std::string selected_quadrics;

    if (contains(type_names[selected_type], "anisotropic")) {
        if (contains(type_names[selected_type], "local")) {
            auto id = state->picker.entity_id;
            if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
                gui_property_selector(state, state->get_vertices(id), {1}, "selected_local_sigma_p",
                                      selected_local_sigma_p);
                gui_property_selector(state, state->get_vertices(id), {1}, "selected_local_sigma_n",
                                      selected_local_sigma_n);
            }
        } else {
            draw_input_vec3(&state->window, "global_sigma_p", anisotropic_sigma_p);
            draw_input_vec3(&state->window, "global_sigma_n", anisotropic_sigma_n);
        }
    } else {
        if (contains(type_names[selected_type], "local")) {
            auto id = state->picker.entity_id;
            if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
                gui_property_selector(state, state->get_vertices(id), {1}, "selected_local_sigma_p",
                                      selected_local_sigma_p);
                gui_property_selector(state, state->get_vertices(id), {1}, "selected_local_sigma_n",
                                      selected_local_sigma_n);
            }
        } else {
            ImGui::InputFloat("global_sigma_p", &isotropic_sigma_p);
            ImGui::InputFloat("global_sigma_n", &isotropic_sigma_n);
        }
    }
    auto id = state->picker.entity_id;
    if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
        gui_property_selector(state, state->get_vertices(id), {3}, "selected_normals", selected_normals);
    }
    if (ImGui::Button("Compute Vertex Quadrics")) {
        if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);
            selected_quadrics = "v_quadric";
            switch (static_cast<MeshVertexQuadricType>(selected_type)) {
                case MeshVertexQuadricType::point : {
                    mesh_vertex_point_quadric(mesh, state->config.parallel_grain_size);
                    break;
                }
                case MeshVertexQuadricType::plane : {
                    mesh_vertex_plane_quadric(mesh, mesh.vertices.get<VectorS<3>, 3>(selected_normals),
                                              state->config.parallel_grain_size);
                    break;
                }
                case MeshVertexQuadricType::local_isotropic_probabilistic_plane : {
                    mesh_vertex_probabilistic_plane_quadric_isotropic(mesh, mesh.vertices.get<VectorS<3>, 3>(
                            selected_normals),
                                                                      mesh.vertices.get<bcg_scalar_t, 1>(
                                                                              selected_local_sigma_p),
                                                                      mesh.vertices.get<bcg_scalar_t, 1>(
                                                                              selected_local_sigma_n),
                                                                      state->config.parallel_grain_size);
                    break;
                }
                case MeshVertexQuadricType::global_isotropic_probabilistic_plane : {
                    mesh_vertex_probabilistic_plane_quadric_isotropic(mesh, mesh.vertices.get<VectorS<3>, 3>(
                            selected_normals), isotropic_sigma_p, isotropic_sigma_n, state->config.parallel_grain_size);
                    break;
                }
                case MeshVertexQuadricType::local_anisotropic_probabilistic_plane : {
                    mesh_vertex_probabilistic_plane_quadric_anisotropic(mesh, mesh.vertices.get<VectorS<3>, 3>(
                            selected_normals),
                                                                        mesh.vertices.get<MatrixS<3, 3>, 1>(
                                                                                selected_local_sigma_p),
                                                                        mesh.vertices.get<MatrixS<3, 3>, 1>(
                                                                                selected_local_sigma_n),
                                                                        state->config.parallel_grain_size);
                    break;
                }
                case MeshVertexQuadricType::global_anisotropic_probabilistic_plane : {
                    mesh_vertex_probabilistic_plane_quadric_anisotropic(mesh, mesh.vertices.get<VectorS<3>, 3>(
                            selected_normals), anisotropic_sigma_p.asDiagonal(), anisotropic_sigma_n.asDiagonal(),
                                                                        state->config.parallel_grain_size);
                    break;
                }
                case MeshVertexQuadricType::__last__ : {
                    break;
                }
            }
        }
    }
    gui_property_selector(state, state->get_vertices(id), {1}, "selected_quadrics",
                          selected_quadrics);
    if (ImGui::Button("Compute Quadric Error")) {
        if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);
            mesh_vertex_quadric_error(mesh, mesh.vertices.get<quadric, 1>(selected_quadrics),
                                      state->config.parallel_grain_size);
            auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
            auto &color = material.attributes[2];
            color.property_name = "v_quadric_error";
            state->dispatcher.trigger<event::mesh_renderer::set_vertex_color_attribute>(state->picker.entity_id, color);
        }
    }
    if (ImGui::Button("Compute Quadric Minimizer")) {
        if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);

            mesh_vertex_quadric_minimizer(mesh, mesh.vertices.get<quadric, 1>(selected_quadrics),
                                          state->config.parallel_grain_size);

        }
    }
    static Params params;
    ImGui::Checkbox("use_area_weighting", &params.use_area_weighting);
    ImGui::Checkbox("use_normal_weighting", &params.use_normal_weighting);
    ImGui::Checkbox("use_distance_weighting", &params.use_distance_weighting);
    if (ImGui::Button("Compute Quadric Sum")) {
        if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);

            mesh_vertex_quadric_neighbors_sum(mesh, mesh.vertices.get<quadric, 1>(selected_quadrics), params,
                                              state->config.parallel_grain_size);
            selected_quadrics = "v_quadric_sum";
        }
    }
    if (ImGui::Button("Compute Quadric Avg")) {
        if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);

            mesh_vertex_quadric_neighbors_avg(mesh, mesh.vertices.get<quadric, 1>(selected_quadrics),
                                              params,
                                              state->config.parallel_grain_size);
            selected_quadrics = "v_quadric_avg";
        }
    }
    if (ImGui::Button("Extract Normals")) {
        if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);

            mesh_vertex_quadric_extract_normals(mesh, mesh.vertices.get<quadric, 1>(selected_quadrics),
                                                state->config.parallel_grain_size);

        }
    }
    ImGui::Separator();
    if(gui_point_cloud_vertex_noise(state)){
        if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);
            vertex_normals(mesh, vertex_normal_area_angle, state->config.parallel_grain_size);
        }
    }
}

}