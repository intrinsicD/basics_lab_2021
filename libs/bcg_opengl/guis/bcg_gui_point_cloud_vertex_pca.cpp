//
// Created by alex on 30.11.20.
//

#include "bcg_gui_point_cloud_vertex_pca.h"
#include "bcg_gui_material_points.h"
#include "bcg_gui_kdtree_selector.h"
#include "renderers/points_renderer/bcg_material_points.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg {

enum class PcaType {
    svd,
    weighted_svd,
    eig,
    weighted_eig,
    __last__
};

static std::vector<std::string> pca_type_names() {
    std::vector<std::string> names(static_cast<int>(PcaType::__last__));
    names[static_cast<int>(PcaType::svd)] = "svd";
    names[static_cast<int>(PcaType::weighted_svd)] = "weighted_svd";
    names[static_cast<int>(PcaType::eig)] = "eig";
    names[static_cast<int>(PcaType::weighted_eig)] = "weighted_eig";
    return names;
}

void gui_point_cloud_vertex_pca(viewer_state *state) {
    static auto names = pca_type_names();
    static int idx = 0;
    static bool compute_mean = true;
    kdtree_parameters params = gui_kd_tree_selector(state);
    ImGui::Separator();
    ImGui::PushID("vertex_pca");
    draw_combobox(&state->window, "pca_type", idx, names);
    ImGui::Checkbox("compute mean", &compute_mean);
    if (ImGui::Button("Compute")) {
        switch (static_cast<PcaType >(idx)) {
            case PcaType::svd : {
                state->dispatcher.trigger<event::point_cloud::vertex::pca::svd>(state->picker.entity_id,
                                                                                compute_mean,
                                                                                params.num_closest,
                                                                                params.radius);
                break;
            }
            case PcaType::weighted_svd : {
                state->dispatcher.trigger<event::point_cloud::vertex::pca::weighted_svd>(state->picker.entity_id,
                                                                                         compute_mean,
                                                                                         params.num_closest,
                                                                                         params.radius);
                break;
            }
            case PcaType::eig : {
                state->dispatcher.trigger<event::point_cloud::vertex::pca::eig>(state->picker.entity_id,
                                                                                compute_mean,
                                                                                params.num_closest,
                                                                                params.radius);
                break;
            }
            case PcaType::weighted_eig : {
                state->dispatcher.trigger<event::point_cloud::vertex::pca::weighted_eig>(state->picker.entity_id,
                                                                                         compute_mean,
                                                                                         params.num_closest,
                                                                                         params.radius);
                break;
            }
            case PcaType::__last__ : {
                return;
            }
        }
    }
    if (state->scene.valid(state->picker.entity_id)) {
        gui_material_points(state, state->scene.try_get<material_points>(state->picker.entity_id),
                            state->picker.entity_id);
    }
    ImGui::PopID();
}

}