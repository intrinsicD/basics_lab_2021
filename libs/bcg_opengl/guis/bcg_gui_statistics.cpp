//
// Created by alex on 08.12.20.
//

#include "bcg_gui_statistics.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg{

void gui_statistics(viewer_state *state, running_stats *stats, const std::vector<float> *values){
    if(!stats) return;

    ImGui::LabelText("num values", "%s", std::to_string(stats->size()).c_str());
    ImGui::LabelText("min", "%s", std::to_string(stats->min()).c_str());
    ImGui::LabelText("max", "%s", std::to_string(stats->max()).c_str());
    ImGui::LabelText("mean", "%s", std::to_string(stats->mean()).c_str());
    ImGui::LabelText("variance", "%s", std::to_string(stats->variance()).c_str());
    ImGui::LabelText("standard_deviation", "%s", std::to_string(stats->standard_deviation()).c_str());
    ImGui::LabelText("skewness", "%s", std::to_string(stats->skewness()).c_str());
    ImGui::LabelText("kurtosis", "%s", std::to_string(stats->kurtosis()).c_str());
    ImGui::LabelText("median", "%s", std::to_string(stats->median()).c_str());

    if(values && !values->empty()){
        draw_histogram(&state->window, "values", *values);
    }
}

}