//
// Created by alex on 23.02.21.
//

#include "bcg_gui_mesh_robust_normal_filtering.h"
#include "bcg_gui_point_cloud_vertex_noise.h"
#include "bcg_gui_property_selector.h"
#include "mesh/bcg_mesh_vertex_normals.h"
#include "mesh/bcg_mesh_face_normals.h"
#include "mesh/bcg_mesh_vertex_quadric.h"
#include "mesh/bcg_mesh_face_quadric.h"
#include "bcg_gui_mesh_offset_mesh.h"
#include "mesh/bcg_mesh_volume.h"
#include "viewer/bcg_viewer_state.h"
#include "utils/bcg_string_utils.h"
#include "bcg_library/geometry/mesh/bcg_mesh_normal_filtering_robust_statistics.h"

namespace bcg {

void gui_mesh_normal_filtering_robust_statistics(viewer_state *state) {
    static float sigma_p = 0.01;
    static float sigma_n = 0.01;

    static VectorS<3> anisotropic_sigma_p = VectorS<3>::Ones() * 0.01;
    static VectorS<3> anisotropic_sigma_n = VectorS<3>::Ones() * 0.01;
    static VectorS<3> anisotropic_sigma_q = VectorS<3>::Ones() * 0.01;
    static VectorS<3> anisotropic_sigma_r = VectorS<3>::Ones() * 0.01;
    static float isotropic_sigma_p = 0.01;
    static float isotropic_sigma_n = 0.01;

    static std::string selected_local_sigma_p;
    static std::string selected_local_sigma_n;
    static float sigma_g = 0.01;
    static float volume_before = 0;
    static float volume_after = 0;
    ImGui::Separator();

    static auto normal_filtering_type_names = mesh_normal_filtering_type_names();
    static int selected_filtering_method = 0;
    static int iterations = 1;
    static bool use_quadric_update = true;
    draw_combobox(&state->window, "Filtering Method", selected_filtering_method, normal_filtering_type_names);
    ImGui::Checkbox("use_quadric_update", &use_quadric_update);
    static auto vertex_quadric_type_names = mesh_vertex_quadric_type_names();
    static auto face_quadric_type_names = mesh_face_quadric_type_names();
    static int selected_vertex_quadric = static_cast<int>(MeshVertexQuadricType::__last__);
    static int selected_face_quadric = static_cast<int>(MeshFaceQuadricType::__last__);
    static Params params;
    if (static_cast<int>(NormalFilteringType::unilateral_probabilistic_quadric) != selected_filtering_method) {
        ImGui::InputFloat("sigma_g", &sigma_g);
    }
    if (use_quadric_update) {
        if (draw_combobox(&state->window, "Vertex Quadric", selected_vertex_quadric, vertex_quadric_type_names)) {
            selected_face_quadric = static_cast<int>(MeshFaceQuadricType::__last__);
        }
        if (draw_combobox(&state->window, "Face Quadric", selected_face_quadric, face_quadric_type_names)) {
            selected_vertex_quadric = static_cast<int>(MeshVertexQuadricType::__last__);
        }
        if (static_cast<int>(NormalFilteringType::unilateral_probabilistic_quadric) == selected_filtering_method) {
            ImGui::InputInt("iterations", &iterations);
            ImGui::InputFloat("sigma_p", &sigma_p);
            ImGui::InputFloat("sigma_n", &sigma_n);
        } else {
            if (static_cast<MeshVertexQuadricType>(selected_vertex_quadric) != MeshVertexQuadricType::__last__) {
                if (contains(vertex_quadric_type_names[selected_vertex_quadric], "probabilistic")) {
                    if (contains(vertex_quadric_type_names[selected_vertex_quadric], "anisotropic")) {
                        if (contains(vertex_quadric_type_names[selected_vertex_quadric], "local")) {
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
                        if (contains(vertex_quadric_type_names[selected_vertex_quadric], "local")) {
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
                }
            }
            if (static_cast<MeshFaceQuadricType>(selected_face_quadric) != MeshFaceQuadricType::__last__) {
                if (contains(face_quadric_type_names[selected_face_quadric], "probabilistic")) {
                    if (contains(face_quadric_type_names[selected_face_quadric], "anisotropic")) {
                        if (contains(face_quadric_type_names[selected_face_quadric], "local")) {
                            auto id = state->picker.entity_id;
                            if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
                                gui_property_selector(state, state->get_vertices(id), {1}, "selected_local_sigma_p",
                                                      selected_local_sigma_p);
                                if (contains(face_quadric_type_names[selected_face_quadric], "triangle")) {

                                } else {
                                    gui_property_selector(state, state->get_vertices(id), {1},
                                                          "selected_local_sigma_n", selected_local_sigma_n);
                                }
                            }
                        } else {
                            draw_input_vec3(&state->window, "global_sigma_p", anisotropic_sigma_p);
                            if (contains(face_quadric_type_names[selected_face_quadric], "triangle")) {
                                draw_input_vec3(&state->window, "global_sigma_q", anisotropic_sigma_q);
                                draw_input_vec3(&state->window, "global_sigma_r", anisotropic_sigma_r);
                            } else {
                                draw_input_vec3(&state->window, "global_sigma_n", anisotropic_sigma_n);
                            }
                        }
                    } else {
                        if (contains(face_quadric_type_names[selected_face_quadric], "local")) {
                            auto id = state->picker.entity_id;
                            if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
                                gui_property_selector(state, state->get_vertices(id), {1}, "selected_local_sigma_p",
                                                      selected_local_sigma_p);
                                if (contains(face_quadric_type_names[selected_face_quadric], "triangle")) {

                                } else {
                                    gui_property_selector(state, state->get_vertices(id), {1},
                                                          "selected_local_sigma_n", selected_local_sigma_n);
                                }
                            }
                        } else {
                            ImGui::InputFloat("global_sigma_p", &isotropic_sigma_p);
                            if (contains(face_quadric_type_names[selected_face_quadric], "triangle")) {

                            } else {
                                ImGui::InputFloat("global_sigma_n", &isotropic_sigma_n);
                            }
                        }
                    }
                }
            }
            ImGui::Separator();

            ImGui::Checkbox("use_area_weighting", &params.use_area_weighting);
            ImGui::Checkbox("use_normal_weighting", &params.use_normal_weighting);
            ImGui::Checkbox("use_distance_weighting", &params.use_distance_weighting);
        }
    }

    static bool update_every_frame = false;
    static int count = 0;
    static int max_count = 0;

    ImGui::InputInt("max_count", &max_count);
    if (ImGui::Checkbox("update_every_frame", &update_every_frame)) {
        if (update_every_frame) {
            count = 0;
        }
    }

    if (max_count > 0 && count > max_count) update_every_frame = false;

    if (update_every_frame || ImGui::Button("Compute Filtering")) {
        auto id = state->picker.entity_id;
        if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);

            if (volume_before < scalar_eps) {
                volume_before = mesh_volume(mesh, state->config.parallel_grain_size);
            }

            switch (static_cast<NormalFilteringType>(selected_filtering_method)) {
                case NormalFilteringType::unilateral_belyaev_ohtake : {
                    mesh_normal_unilateral_filtering_belyaev_ohtake(mesh, sigma_g, state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::unilateral_yagou_mean : {
                    mesh_normal_unilateral_filtering_yagou_mean(mesh, sigma_g, state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::unilateral_yagou_median : {
                    mesh_normal_unilateral_filtering_yagou_median(mesh, sigma_g, state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::unilateral_yadav : {
                    mesh_normal_unilateral_filtering_yadav(mesh, sigma_g, state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::unilateral_shen : {
                    mesh_normal_unilateral_filtering_shen(mesh, sigma_g, state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::unilateral_tasdizen : {
                    mesh_normal_unilateral_filtering_tasdizen(mesh, sigma_g, state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::unilateral_centin : {
                    mesh_normal_unilateral_filtering_centin(mesh, sigma_g, state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::unilateral_probabilistic_quadric : {
                    mesh_normal_unilateral_filtering_probabilistic_quadric(mesh, iterations, sigma_p, sigma_n,
                                                                           state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::bilateral_zheng : {
                    mesh_normal_bilateral_filtering_zheng(mesh, sigma_g, state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::bilateral_zhang : {
                    mesh_normal_bilateral_filtering_zhang(mesh, sigma_g, state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::bilateral_yadav : {
                    mesh_normal_bilateral_filtering_yadav(mesh, sigma_g, state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::__last__ : {
                    break;
                }
            }
            std::cout << "computed filtered normals\n";
            auto f_normals = mesh.faces.get<VectorS<3>, 3>("f_normal_filtered");
            vertex_normals(mesh, f_normals, state->config.parallel_grain_size);
            if (use_quadric_update &&
                static_cast<MeshVertexQuadricType>(selected_vertex_quadric) != MeshVertexQuadricType::__last__) {
                auto normals = mesh.vertices.get<VectorS<3>, 3>("v_normal");
                switch (static_cast<MeshVertexQuadricType>(selected_vertex_quadric)) {
                    case MeshVertexQuadricType::point : {
                        mesh_vertex_point_quadric(mesh, state->config.parallel_grain_size);
                        break;
                    }
                    case MeshVertexQuadricType::plane : {
                        mesh_vertex_plane_quadric(mesh, normals, state->config.parallel_grain_size);
                        break;
                    }
                    case MeshVertexQuadricType::local_isotropic_probabilistic_plane : {
                        mesh_vertex_probabilistic_plane_quadric_isotropic(mesh, normals,
                                                                          mesh.vertices.get<bcg_scalar_t, 1>(
                                                                                  selected_local_sigma_p),
                                                                          mesh.vertices.get<bcg_scalar_t, 1>(
                                                                                  selected_local_sigma_n),
                                                                          state->config.parallel_grain_size);
                        break;
                    }
                    case MeshVertexQuadricType::global_isotropic_probabilistic_plane : {
                        mesh_vertex_probabilistic_plane_quadric_isotropic(mesh, normals, isotropic_sigma_p,
                                                                          isotropic_sigma_n,
                                                                          state->config.parallel_grain_size);
                        break;
                    }
                    case MeshVertexQuadricType::local_anisotropic_probabilistic_plane : {
                        mesh_vertex_probabilistic_plane_quadric_anisotropic(mesh, normals,
                                                                            mesh.vertices.get<MatrixS<3, 3>, 1>(
                                                                                    selected_local_sigma_p),
                                                                            mesh.vertices.get<MatrixS<3, 3>, 1>(
                                                                                    selected_local_sigma_n),
                                                                            state->config.parallel_grain_size);
                        break;
                    }
                    case MeshVertexQuadricType::global_anisotropic_probabilistic_plane : {
                        mesh_vertex_probabilistic_plane_quadric_anisotropic(mesh, normals,
                                                                            anisotropic_sigma_p.asDiagonal(),
                                                                            anisotropic_sigma_n.asDiagonal(),
                                                                            state->config.parallel_grain_size);
                        break;
                    }
                    case MeshVertexQuadricType::__last__ : {
                        break;
                    }
                }
                auto quadrics = mesh.vertices.get<quadric, 1>("v_quadric");
                if(quadrics) {
                    std::cout << "computed vertex quadrics from vertex normals\n";
                }
                mesh_vertex_quadric_neighbors_avg(mesh, quadrics, params, state->config.parallel_grain_size);
                auto quadric_avg = mesh.vertices.get<quadric, 1>("v_quadric_avg");
                mesh_vertex_quadric_minimizer(mesh, quadric_avg, state->config.parallel_grain_size);
                std::cout << "computed vertex quadrics update\n";
            } else if (use_quadric_update &&
                       static_cast<MeshFaceQuadricType>(selected_face_quadric) != MeshFaceQuadricType::__last__) {

                switch (static_cast<MeshFaceQuadricType>(selected_face_quadric)) {
                    case MeshFaceQuadricType::point : {
                        mesh_face_point_quadric(mesh, state->config.parallel_grain_size);
                        break;
                    }
                    case MeshFaceQuadricType::plane : {
                        mesh_face_plane_quadric(mesh, f_normals, state->config.parallel_grain_size);
                        break;
                    }
                    case MeshFaceQuadricType::triangle : {
                        mesh_face_triangle_quadric(mesh, state->config.parallel_grain_size);
                        break;
                    }
                    case MeshFaceQuadricType::local_isotropic_probabilistic_plane : {
                        mesh_face_probabilistic_plane_quadric_isotropic(mesh, f_normals,
                                                                        mesh.faces.get<bcg_scalar_t, 1>(
                                                                                selected_local_sigma_p),
                                                                        mesh.faces.get<bcg_scalar_t, 1>(
                                                                                selected_local_sigma_n),
                                                                        state->config.parallel_grain_size);
                        break;
                    }
                    case MeshFaceQuadricType::global_isotropic_probabilistic_plane : {
                        mesh_face_probabilistic_plane_quadric_isotropic(mesh, f_normals, isotropic_sigma_p,
                                                                        isotropic_sigma_n,
                                                                        state->config.parallel_grain_size);
                        break;
                    }
                    case MeshFaceQuadricType::local_anisotropic_probabilistic_plane : {
                        mesh_face_probabilistic_plane_quadric_anisotropic(mesh, f_normals,
                                                                          mesh.faces.get<MatrixS<3, 3>, 1>(
                                                                                  selected_local_sigma_p),
                                                                          mesh.faces.get<MatrixS<3, 3>, 1>(
                                                                                  selected_local_sigma_n),
                                                                          state->config.parallel_grain_size);
                        break;
                    }
                    case MeshFaceQuadricType::global_anisotropic_probabilistic_plane : {
                        mesh_face_probabilistic_plane_quadric_anisotropic(mesh, f_normals,
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
                std::cout << "computed face quadrics from filtered face normals\n";
                auto quadrics = mesh.faces.get<quadric, 1>("f_quadric");
                mesh_face_quadric_neighbors_avg_to_vertices(mesh, quadrics, params, state->config.parallel_grain_size);
                std::cout << "computed vertex quadrics from averaged face quadrics\n";
                auto quadric_avg = mesh.vertices.get<quadric, 1>("v_quadric_avg");
                mesh_vertex_quadric_minimizer(mesh, quadric_avg, state->config.parallel_grain_size);
                std::cout << "computed vertex quadrics update\n";
            } else {
                mesh_normal_ohtake_vertex_update(mesh, f_normals, state->config.parallel_grain_size);
            }
            vertex_normals(mesh, vertex_normal_area_angle, state->config.parallel_grain_size);
            volume_after = mesh_volume(mesh, state->config.parallel_grain_size);
        }
        ++count;
    }
    ImGui::LabelText("iterations", "%d", count);
    ImGui::LabelText("volume_before", "%f", volume_before);
    ImGui::LabelText("volume_after", "%f", volume_after);
    ImGui::LabelText("volume_shrinkage", "%f", volume_after / volume_before);
    if (ImGui::Button("Update volume_before")) {
        auto id = state->picker.entity_id;
        if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);
            volume_before = mesh_volume(mesh, state->config.parallel_grain_size);
        }
    }

    if (gui_point_cloud_vertex_noise(state)) {
        auto id = state->picker.entity_id;
        if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);
            vertex_normals(mesh, vertex_normal_area_angle, state->config.parallel_grain_size);
        }

    }
    ImGui::Separator();
    gui_mesh_offset_mesh(state);
}

}