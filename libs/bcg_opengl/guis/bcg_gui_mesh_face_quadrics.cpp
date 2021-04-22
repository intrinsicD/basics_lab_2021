//
// Created by alex on 19.04.21.
//

#include "bcg_gui_mesh_face_quadrics.h"
#include "bcg_gui_property_selector.h"
#include "bcg_gui_point_cloud_vertex_noise.h"
#include "mesh/bcg_mesh_face_quadric.h"
#include "mesh/bcg_mesh_vertex_quadric.h"
#include "mesh/bcg_mesh_vertex_normals.h"
#include "mesh/bcg_mesh_face_normals.h"
#include "viewer/bcg_viewer_state.h"
#include "utils/bcg_string_utils.h"
#include "renderers/mesh_renderer/bcg_events_mesh_renderer.h"
#include "renderers/mesh_renderer/bcg_material_mesh.h"

namespace bcg {

void gui_mesh_face_quadrics(viewer_state *state) {
    static auto type_names = mesh_face_quadric_type_names();
    static int selected_type = 0;
    draw_combobox(&state->window, "quadric_type", selected_type, type_names);

    static VectorS<3> anisotropic_sigma_p = VectorS<3>::Ones() * 0.01;
    static VectorS<3> anisotropic_sigma_n = VectorS<3>::Ones() * 0.01;
    static VectorS<3> anisotropic_sigma_q = VectorS<3>::Ones() * 0.01;
    static VectorS<3> anisotropic_sigma_r = VectorS<3>::Ones() * 0.01;
    static float isotropic_sigma_p = 0.01;
    static float isotropic_sigma_n = 0.01;

    static std::string selected_local_sigma_p;
    static std::string selected_local_sigma_n;
    static std::string selected_quadrics_faces;
    static std::string selected_quadrics_vertices;

    if (contains(type_names[selected_type], "probabilistic")) {
        if (contains(type_names[selected_type], "anisotropic")) {
            if (contains(type_names[selected_type], "local")) {
                auto id = state->picker.entity_id;
                if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
                    gui_property_selector(state, state->get_vertices(id), {1}, "selected_local_sigma_p",
                                          selected_local_sigma_p);
                    if (contains(type_names[selected_type], "triangle")) {

                    } else {
                        gui_property_selector(state, state->get_vertices(id), {1},
                                              "selected_local_sigma_n", selected_local_sigma_n);
                    }
                }
            } else {
                draw_input_vec3(&state->window, "global_sigma_p", anisotropic_sigma_p);
                if (contains(type_names[selected_type], "triangle")) {
                    draw_input_vec3(&state->window, "global_sigma_q", anisotropic_sigma_q);
                    draw_input_vec3(&state->window, "global_sigma_r", anisotropic_sigma_r);
                } else {
                    draw_input_vec3(&state->window, "global_sigma_n", anisotropic_sigma_n);
                }
            }
        } else {
            if (contains(type_names[selected_type], "local")) {
                auto id = state->picker.entity_id;
                if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
                    gui_property_selector(state, state->get_vertices(id), {1}, "selected_local_sigma_p",
                                          selected_local_sigma_p);
                    if (contains(type_names[selected_type], "triangle")) {

                    } else {
                        gui_property_selector(state, state->get_vertices(id), {1},
                                              "selected_local_sigma_n", selected_local_sigma_n);
                    }
                }
            } else {
                ImGui::InputFloat("global_sigma_p", &isotropic_sigma_p);
                if (contains(type_names[selected_type], "triangle")) {

                } else {
                    ImGui::InputFloat("global_sigma_n", &isotropic_sigma_n);
                }
            }
        }
    }

    auto id = state->picker.entity_id;
    if (ImGui::Button("Compute Face Quadrics")) {
        if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);
            face_normals(mesh, state->config.parallel_grain_size);
            auto normals = mesh.faces.get<VectorS<3>, 3>("f_normal");
            selected_quadrics_faces = "f_quadric";
            switch (static_cast<MeshFaceQuadricType>(selected_type)) {
                case MeshFaceQuadricType::point : {
                    mesh_face_point_quadric(mesh, state->config.parallel_grain_size);
                    break;
                }
                case MeshFaceQuadricType::plane : {
                    mesh_face_plane_quadric(mesh, normals, state->config.parallel_grain_size);
                    break;
                }
                case MeshFaceQuadricType::triangle : {
                    mesh_face_triangle_quadric(mesh, state->config.parallel_grain_size);
                    break;
                }
                case MeshFaceQuadricType::local_isotropic_probabilistic_plane : {
                    mesh_face_probabilistic_plane_quadric_isotropic(mesh, normals,
                                                                    mesh.faces.get<bcg_scalar_t, 1>(
                                                                            selected_local_sigma_p),
                                                                    mesh.faces.get<bcg_scalar_t, 1>(
                                                                            selected_local_sigma_n),
                                                                    state->config.parallel_grain_size);
                    break;
                }
                case MeshFaceQuadricType::global_isotropic_probabilistic_plane : {
                    mesh_face_probabilistic_plane_quadric_isotropic(mesh, normals, isotropic_sigma_p,
                                                                    isotropic_sigma_n,
                                                                    state->config.parallel_grain_size);
                    break;
                }
                case MeshFaceQuadricType::local_anisotropic_probabilistic_plane : {
                    mesh_face_probabilistic_plane_quadric_anisotropic(mesh, normals,
                                                                      mesh.faces.get<MatrixS<3, 3>, 1>(
                                                                              selected_local_sigma_p),
                                                                      mesh.faces.get<MatrixS<3, 3>, 1>(
                                                                              selected_local_sigma_n),
                                                                      state->config.parallel_grain_size);
                    break;
                }
                case MeshFaceQuadricType::global_anisotropic_probabilistic_plane : {
                    mesh_face_probabilistic_plane_quadric_anisotropic(mesh, normals,
                                                                      anisotropic_sigma_p.asDiagonal(),
                                                                      anisotropic_sigma_n.asDiagonal(),
                                                                      state->config.parallel_grain_size);
                    break;
                }
                case MeshFaceQuadricType::local_anisotropic_probabilistic_triangle : {
                    mesh_face_probabilistic_triangle_quadric_anisotropic(mesh,
                                                                         mesh.faces.get<MatrixS<3, 3>, 1>(
                                                                                 selected_local_sigma_p),
                                                                         state->config.parallel_grain_size);
                    break;
                }
                case MeshFaceQuadricType::local_isotropic_probabilistic_triangle : {
                    mesh_face_probabilistic_triangle_quadric_isotropic(mesh,
                                                                       mesh.faces.get<bcg_scalar_t, 1>(
                                                                               selected_local_sigma_p),
                                                                       state->config.parallel_grain_size);
                    break;
                }
                case MeshFaceQuadricType::global_isotropic_probabilistic_triangle : {
                    mesh_face_probabilistic_triangle_quadric_isotropic(mesh,
                                                                       isotropic_sigma_p,
                                                                       state->config.parallel_grain_size);
                    break;
                }
                case MeshFaceQuadricType::global_anisotropic_probabilistic_triangle : {
                    mesh_face_probabilistic_triangle_quadric_anisotropic(mesh, anisotropic_sigma_p.asDiagonal(),
                                                                         anisotropic_sigma_q.asDiagonal(),
                                                                         anisotropic_sigma_r.asDiagonal(),
                                                                         state->config.parallel_grain_size);
                    break;
                }
                case MeshFaceQuadricType::__last__ : {
                    break;
                }
            }
        }
    }

    if (!selected_quadrics_faces.empty()) {
        ImGui::Separator();
        gui_property_selector(state, state->get_faces(id), {1}, "selected_quadrics_faces",
                              selected_quadrics_faces);
        if (!selected_quadrics_vertices.empty()) {
            gui_property_selector(state, state->get_vertices(id), {1}, "selected_quadrics_vertices",
                                  selected_quadrics_vertices);
        }
        if (ImGui::Button("Compute Error")) {
            if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
                auto &mesh = state->scene.get<halfedge_mesh>(id);
                mesh_face_quadric_error(mesh, mesh.faces.get<quadric, 1>(selected_quadrics_faces),
                                        state->config.parallel_grain_size);
                auto &material = state->scene.get<material_mesh>(state->picker.entity_id);
                auto &color = material.attributes[2];
                color.property_name = "f_quadric_error";
                state->dispatcher.trigger<event::mesh_renderer::set_face_color_attribute>(state->picker.entity_id,
                                                                                          color);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Compute Minimizer")) {
            if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
                auto &mesh = state->scene.get<halfedge_mesh>(id);
                if (selected_quadrics_vertices == "v_quadric_avg" || selected_quadrics_vertices == "v_quadric_sum") {
                    mesh_vertex_quadric_minimizer(mesh,
                                                  mesh.vertices.get<quadric, 1>(selected_quadrics_vertices),
                                                  state->config.parallel_grain_size);
                } else {
                    mesh_face_quadric_minimizer(mesh, mesh.faces.get<quadric, 1>(selected_quadrics_faces),
                                                state->config.parallel_grain_size);
                }

            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Extract Normals")) {
            if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
                auto &mesh = state->scene.get<halfedge_mesh>(id);

                mesh_face_quadric_extract_normals(mesh, mesh.faces.get<quadric, 1>(selected_quadrics_faces),
                                                  state->config.parallel_grain_size);
            }
        }
        ImGui::Separator();
        static Params params;
        ImGui::Checkbox("use_area_weighting", &params.use_area_weighting);
        ImGui::Checkbox("use_normal_weighting", &params.use_normal_weighting);
        ImGui::Checkbox("use_distance_weighting", &params.use_distance_weighting);
        if (ImGui::Button("Sum")) {
            if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
                auto &mesh = state->scene.get<halfedge_mesh>(id);

                mesh_face_quadric_neighbors_sum(mesh, mesh.faces.get<quadric, 1>(selected_quadrics_faces),
                                                params, state->config.parallel_grain_size);
                selected_quadrics_faces = "f_quadric_sum";
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Average")) {
            if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
                auto &mesh = state->scene.get<halfedge_mesh>(id);

                mesh_face_quadric_neighbors_avg(mesh, mesh.faces.get<quadric, 1>(selected_quadrics_faces),
                                                params, state->config.parallel_grain_size);
                selected_quadrics_faces = "f_quadric_avg";
            }
        }
        if (ImGui::Button("Sum to Vertices")) {
            if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
                auto &mesh = state->scene.get<halfedge_mesh>(id);

                mesh_face_quadric_neighbors_sum_to_vertices(mesh,
                                                            mesh.faces.get<quadric, 1>(selected_quadrics_faces),
                                                            params, state->config.parallel_grain_size);
                selected_quadrics_vertices = "v_quadric_sum";
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Average to Vertices")) {
            if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
                auto &mesh = state->scene.get<halfedge_mesh>(id);

                mesh_face_quadric_neighbors_avg_to_vertices(mesh,
                                                            mesh.faces.get<quadric, 1>(selected_quadrics_faces),
                                                            params, state->config.parallel_grain_size);
                selected_quadrics_vertices = "v_quadric_avg";
            }
        }
    }

    ImGui::Separator();
    if (gui_point_cloud_vertex_noise(state)) {
        if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);
            vertex_normals(mesh, vertex_normal_area_angle, state->config.parallel_grain_size);
        }
    }
}

}