//
// Created by alex on 24.02.21.
//

#include "bcg_gui_point_cloud_normal_filtering_robust_statistics.h"
#include "geometry/point_cloud/bcg_point_cloud_normal_filtering_robust_statistics.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_point_cloud_vertex_noise.h"

namespace bcg {

void gui_point_cloud_normal_filtering_robust_statistics(viewer_state *state) {
    static auto type_names = point_cloud_normal_filtering_type_names();
    static int e = 0;
    static int num_closest = 12;
    static float radius = 0.01;
    static float sigma_g = 0.1;
    static float sigma_p = 0.1;
    static float sigma_n = 0.1;
    static float lambda = 0.1;
    static bool use_repulsion_tangential = false;
    static bool use_attraction_normal = false;

    draw_combobox(&state->window, "method_type", e, type_names);

    if (static_cast<PointCloudNormalFilteringType>(e) == PointCloudNormalFilteringType::averaging_knn) {
        ImGui::InputInt("num_closest", &num_closest);
    }

    if (static_cast<PointCloudNormalFilteringType>(e) == PointCloudNormalFilteringType::averaging_radius) {
        ImGui::InputFloat("radius", &radius);
    }
    ImGui::InputFloat("sigma_g", &sigma_g);
    ImGui::InputFloat("sigma_p", &sigma_p);
    ImGui::InputFloat("sigma_n", &sigma_n);
    ImGui::InputFloat("lambda", &lambda);
    ImGui::Checkbox("use_repulsion_tangential", &use_repulsion_tangential);
    ImGui::Checkbox("use_attraction_normal", &use_attraction_normal);
    if (ImGui::Button("Compute normal filtering")) {
        auto id = state->picker.entity_id;
        if (state->scene.valid(id)) {
            auto *vertices = state->get_vertices(id);
            if (vertices) {
                if (!state->scene.has<kdtree_property<bcg_scalar_t >>(id)) {
                    state->dispatcher.trigger<event::spatial_index::setup_kdtree>(id);
                }
                auto &index = state->scene.get<kdtree_property<bcg_scalar_t>>(id);

                if (static_cast<PointCloudNormalFilteringType>(e) == PointCloudNormalFilteringType::averaging_knn) {
                    point_cloud_normal_filtering_robust_statistics_knn(vertices, index, num_closest, sigma_g, sigma_p,
                                                                       sigma_n, lambda, use_repulsion_tangential,
                                                                       use_attraction_normal,
                                                                       state->config.parallel_grain_size);
                }

                if (static_cast<PointCloudNormalFilteringType>(e) == PointCloudNormalFilteringType::averaging_radius) {
                    point_cloud_normal_filtering_robust_statistics_radius(vertices, index, radius, sigma_p, sigma_n,
                                                                          state->config.parallel_grain_size);
                }
            }
        }
    }
    ImGui::Separator();
    gui_point_cloud_vertex_noise(state);
}

}