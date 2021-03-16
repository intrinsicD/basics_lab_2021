//
// Created by alex on 17.11.20.
//

#include "bcg_gui_viewer_picker.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg {

void gui_viewer_picker(viewer_state *state) {
    if (ImGui::CollapsingHeader("Picker")) {
        static int idx = 0;
        static std::vector<std::string> names = {"disabled", "point", "vertex", "edges", "faces"};
        if (draw_combobox(&state->window, "picker mode", idx, names)) {
            switch (idx){
                case 0 : {
                    state->dispatcher.trigger<event::picker::disable>();
                    break;
                }
                case 1 : {
                    state->dispatcher.trigger<event::picker::enable::point>();
                    break;
                }
                case 2 : {
                    state->dispatcher.trigger<event::picker::enable::vertex>();
                    break;
                }
                case 3 : {
                    state->dispatcher.trigger<event::picker::enable::edge>();
                    break;
                }
                case 4 : {
                    state->dispatcher.trigger<event::picker::enable::face>();
                    break;
                }
                default : {

                }
            }
        }
        if (ImGui::CollapsingHeader("Picker Info")) {
            draw_label(&state->window, "valid", std::to_string(state->picker.valid));
            draw_label(&state->window, "entity_id", std::to_string((unsigned int) state->picker.entity_id));
            draw_label(&state->window, "vertex_id", std::to_string(state->picker.vertex_id.idx));
            draw_label(&state->window, "edge_id", std::to_string(state->picker.edge_id.idx));
            draw_label(&state->window, "face_id", std::to_string(state->picker.face_id.idx));
            ImGui::Separator();
            std::stringstream ss;
            ss << state->picker.model_space_point.transpose();
            draw_label(&state->window, "model_space_point", ss.str());
            ss.str("");
            ss << state->picker.world_space_point.transpose();
            draw_label(&state->window, "world_space_point", ss.str());
            ss.str("");
            ss << state->picker.view_space_point.transpose();
            draw_label(&state->window, "view_space_point", ss.str());
            ImGui::Separator();
        }
    }

}

}