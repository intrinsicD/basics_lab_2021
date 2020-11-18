//
// Created by alex on 18.11.20.
//

#include <vector>
#include <string>
#include "bcg_gui_viewer_curve.h"
#include "bcg_viewer_state.h"

namespace bcg{

void gui_viewer_curve(viewer_state *state){
    static int e = 0;
    if(ImGui::RadioButton("2D", &e, 0)){
        state->dispatcher.trigger<event::curve::enable_2d_drawing>();
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("3D", &e, 1)){
        state->dispatcher.trigger<event::curve::enable_3d_drawing>();
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("disabled", &e, 2)){
        state->dispatcher.trigger<event::curve::disable_drawing>();
    }
    if(e == 2) return;
    if(ImGui::Button("new curve")) {
        state->dispatcher.trigger<event::curve::make_curve>();
    }
    ImGui::Separator();

}


}

}