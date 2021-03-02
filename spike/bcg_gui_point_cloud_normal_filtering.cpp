//
// Created by alex on 01.03.21.
//

#include "bcg_gui_point_cloud_normal_filtering.h"
#include "bcg_opengl/bcg_viewer_state.h"
#include "bcg_point_cloud_normal_filtering.h"
#include "guis/bcg_gui_point_cloud_vertex_noise.h"

namespace bcg {

void gui_point_cloud_normal_filtering(viewer_state *state) {
    gui_point_cloud_vertex_noise(state);
    ImGui::Separator();
    static int selected_search_type = 0;
    static float radius = 0.01;
    static float diagnal_percent = 1;
    static int num_closest = 12;
    draw_combobox(&state->window, "search_type", selected_search_type, search_type_names());
    if (static_cast<SearchType>(selected_search_type) == SearchType::knn) {
        ImGui::InputInt("num_closest", &num_closest);
    } else {
        ImGui::InputFloat("radius", &radius);
        ImGui::InputFloat("%diagonal", &diagnal_percent);
        if (ImGui::Button("Set Radius to %diagonal of aabb")) {
            auto id = state->picker.entity_id;
            if (state->scene.valid(id)) {
                auto &aabb = state->scene.get<aligned_box3>(id);
                radius = aabb.diagonal().norm() * diagnal_percent / bcg_scalar_t(100);
            }
        }
    }

    ImGui::Separator();

    static float sigma_f = 0.1;
    static float sigma_g = 0.1;
    static int iterations = 1;
    ImGui::InputFloat("sigma_f", &sigma_f);
    ImGui::InputFloat("sigma_g", &sigma_g);
    ImGui::InputInt("iterations", &iterations);

    if (ImGui::Button("Compute Method")) {
        auto id = state->picker.entity_id;
        if (state->scene.valid(id)) {
            auto *vertices = state->get_vertices(id);
            if (vertices) {
                auto &index = state->scene.get<kdtree_property<bcg_scalar_t>>(id);
                pc_search_params search_params{static_cast<SearchType>(selected_search_type), bcg_scalar_t(radius),
                                               num_closest};
                pc_nf_parameters params{bcg_scalar_t(sigma_f), bcg_scalar_t(sigma_g), iterations, search_params};
                point_cloud_normal_filtering(vertices, index, params, state->config.parallel_grain_size);
            }
        }
    }
}

}