//
// Created by alex on 18.11.20.
//

#include <string>
#include "bcg_gui_viewer_curve.h"
#include "bcg_gui_graph.h"
#include "bcg_gui_rendering_options.h"
#include "viewer/bcg_viewer_state.h"
#include "geometry/curve/bcg_curve_bezier.h"

namespace bcg{

void gui_viewer_curve(viewer_state *state){
    static int e = 0;
    if(ImGui::RadioButton("disabled", &e, 0)){
        state->dispatcher.trigger<event::curve::disable_drawing>();
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("2D", &e, 1)){
        state->dispatcher.trigger<event::curve::enable_2d_drawing>();
    }
/*    ImGui::SameLine();
    if(ImGui::RadioButton("3D", &e, 2)){
        state->dispatcher.trigger<event::curve::enable_3d_drawing>();
    }*/
    draw_checkbox(&state->window, "fixed camera", state->keyboard.no_modifier, true);
    if(e == 0) return;
    static entt::entity id = entt::null;
    if(ImGui::Button("new curve")) {
        state->dispatcher.trigger<event::curve::make>();
        id = state->picker.entity_id;
    }
    ImGui::Separator();
    if(state->scene.valid(id) && state->scene.has<curve_bezier>(id)){
        auto &curve = state->scene.get<curve_bezier>(id);
        draw_label(&state->window, "curve id", std::to_string(int(id)));
        draw_label(&state->window, "finished", (curve.finished ? "1" : "0"));
        draw_label(&state->window, "is 3d", (curve.is_3d ? "1" : "0"));
        draw_label(&state->window, "points_clicked", std::to_string(curve.points_clicked));
        gui_graph(state, &curve);
        gui_rendering_options(state, id);
    }


}

}