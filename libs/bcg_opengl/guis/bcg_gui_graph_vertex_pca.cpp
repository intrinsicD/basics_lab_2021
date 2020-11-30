//
// Created by alex on 30.11.20.
//

#include "bcg_gui_graph_vertex_pca.h"
#include "bcg_viewer_state.h"


namespace bcg {

void gui_graph_vertex_pca(viewer_state *state) {
    static auto names = pca_type_names();
    static int idx = 0;
    static bool compute_mean = true;
    draw_combobox(&state->window, "pca_type", idx, names);
    ImGui::Checkbox("compute mean", &compute_mean);
    if (ImGui::Button("Compute")) {
        switch (idx) {
            case static_cast<int>(PcaType::svd) : {
                state->dispatcher.trigger<event::graph::vertex::pca>(state->picker.entity_id, PcaType::svd,
                                                                     compute_mean);
                break;
            }
            case static_cast<int>(PcaType::weighted_svd) : {
                state->dispatcher.trigger<event::graph::vertex::pca>(state->picker.entity_id, PcaType::weighted_svd,
                                                                     compute_mean);
                break;
            }
            case static_cast<int>(PcaType::eig) : {
                state->dispatcher.trigger<event::graph::vertex::pca>(state->picker.entity_id, PcaType::eig,
                                                                     compute_mean);
                break;
            }
            case static_cast<int>(PcaType::weighted_eig) : {
                state->dispatcher.trigger<event::graph::vertex::pca>(state->picker.entity_id, PcaType::weighted_eig,
                                                                     compute_mean);
                break;
            }
        }
    }
}

}