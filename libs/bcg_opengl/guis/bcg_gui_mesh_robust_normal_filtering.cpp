//
// Created by alex on 23.02.21.
//

#include "bcg_gui_mesh_robust_normal_filtering.h"
#include "bcg_gui_point_cloud_vertex_noise.h"
#include "mesh/bcg_mesh_vertex_normals.h"
#include "bcg_gui_mesh_offset_mesh.h"
#include "mesh/bcg_mesh_volume.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_library/geometry/mesh/bcg_mesh_normal_filtering_robust_statistics.h"

namespace bcg {

void gui_mesh_normal_filtering_robust_statistics(viewer_state *state) {
    static float sigma_p = 0.01;
    static float sigma_n = 0.01;
    static float sigma_g = 0.01;
    static float volume_before = 0;
    static float volume_after = 0;
    ImGui::Separator();

    static auto method_names = normal_filtering_names();
    static int e = 0;
    static int iterations = 1;
    static bool use_quadric_update = true;
    draw_combobox(&state->window, "Method", e, method_names);
    ImGui::Checkbox("use_quadric_update", &use_quadric_update);
    if (static_cast<int>(NormalFilteringType::unilateral_probabilistic_quadric) == e) {
        ImGui::InputInt("iterations", &iterations);
        if(use_quadric_update){
            ImGui::InputFloat("sigma_p", &sigma_p);
            ImGui::InputFloat("sigma_n", &sigma_n);
        }
    } else {
        ImGui::InputFloat("sigma_g", &sigma_g);
        if(use_quadric_update){
            ImGui::InputFloat("sigma_p", &sigma_p);
            ImGui::InputFloat("sigma_n", &sigma_n);
        }
    }

    static bool update_every_frame = false;
    static int count = 0;
    static int max_count = 0;

    ImGui::InputInt("max_count", &max_count);
    if(ImGui::Checkbox("update_every_frame", &update_every_frame)){
        if(update_every_frame){
            count = 0;
        }
    }

    if(max_count > 0 && count > max_count) update_every_frame = false;

    if (update_every_frame || ImGui::Button("Compute Filtering")) {

        auto id = state->picker.entity_id;
        if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);

            if(volume_before < scalar_eps){
                volume_before = mesh_volume(mesh, state->config.parallel_grain_size);
            }

            switch (static_cast<NormalFilteringType>(e)) {
                case NormalFilteringType::unilateral_belyaev_ohtake : {
                    mesh_normal_unilateral_filtering_belyaev_ohtake(mesh, sigma_g, sigma_p, sigma_n, use_quadric_update,
                                                                    state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::unilateral_yagou_mean : {
                    mesh_normal_unilateral_filtering_yagou_mean(mesh, sigma_g, sigma_p, sigma_n, use_quadric_update,
                                                                state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::unilateral_yagou_median : {
                    mesh_normal_unilateral_filtering_yagou_median(mesh, sigma_g, sigma_p, sigma_n, use_quadric_update,
                                                                  state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::unilateral_yadav : {
                    mesh_normal_unilateral_filtering_yadav(mesh, sigma_g, sigma_p, sigma_n, use_quadric_update,
                                                           state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::unilateral_shen : {
                    mesh_normal_unilateral_filtering_shen(mesh, sigma_g, sigma_p, sigma_n, use_quadric_update,
                                                          state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::unilateral_tasdizen : {
                    mesh_normal_unilateral_filtering_tasdizen(mesh, sigma_g, sigma_p, sigma_n, use_quadric_update,
                                                              state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::unilateral_centin : {
                    mesh_normal_unilateral_filtering_centin(mesh, sigma_g, sigma_p, sigma_n, use_quadric_update,
                                                            state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::unilateral_probabilistic_quadric : {
                    mesh_normal_unilateral_filtering_probabilistic_quadric(mesh, iterations, sigma_p, sigma_n,
                                                                           state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::bilateral_zheng : {
                    mesh_normal_bilateral_filtering_zheng(mesh, sigma_g, sigma_p, sigma_n, use_quadric_update,
                                                          state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::bilateral_zhang : {
                    mesh_normal_bilateral_filtering_zhang(mesh, sigma_g, sigma_p, sigma_n, use_quadric_update,
                                                          state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::bilateral_yadav : {
                    mesh_normal_bilateral_filtering_yadav(mesh, sigma_g, sigma_p, sigma_n, use_quadric_update,
                                                          state->config.parallel_grain_size);
                    break;
                }
                case NormalFilteringType::__last__ : {
                    break;
                }
            }

            volume_after = mesh_volume(mesh, state->config.parallel_grain_size);
        }
        ++count;
    }
    ImGui::LabelText("iterations", "%d", count);
    ImGui::LabelText("volume_before", "%f", volume_before);
    ImGui::LabelText("volume_after", "%f", volume_after);
    ImGui::LabelText("volume_shrinkage", "%f", volume_after / volume_before);
    if(ImGui::Button("Update volume_before")){
        auto id = state->picker.entity_id;
        if (state->scene.valid(id) && state->scene.all_of<halfedge_mesh>(id)) {
            auto &mesh = state->scene.get<halfedge_mesh>(id);
            volume_before = mesh_volume(mesh, state->config.parallel_grain_size);
        }
    }

    if(gui_point_cloud_vertex_noise(state)){
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