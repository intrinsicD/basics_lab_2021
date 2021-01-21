//
// Created by alex on 21.01.21.
//

#include "bcg_gui_locally_optimal_projection.h"
#include "bcg_viewer_state.h"
#include "sampling/bcg_sampling_locally_optimal_projection.h"

namespace bcg{

void gui_locally_optimal_projection(viewer_state *state){
    static float attraction_radius = 0;
    static float repulsion_weight = 0;
    ImGui::InputFloat("attraction_radius", &attraction_radius);
    if(ImGui::InputFloat("repulsion_weight", &repulsion_weight)){
        repulsion_weight = std::max(0.0f, std::min(repulsion_weight, 0.5f));
    }
    static std::vector<std::string> names = method_names();
    static int idx = 0;

    draw_combobox(&state->window, "method", idx, names);

    if(names[idx] == "custom" && ImGui::CollapsingHeader("Advanced")){
        //TODO select callbacks and stuff, combine as you like
    }
}

}