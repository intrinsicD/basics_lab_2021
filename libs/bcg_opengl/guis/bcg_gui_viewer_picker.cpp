//
// Created by alex on 17.11.20.
//

#include "bcg_gui_viewer_picker.h"
#include "bcg_gui_selected_entities.h"
#include "viewer/bcg_viewer_state.h"
#include "components/bcg_component_selection_vertex_overlay.h"
#include "renderers/points_renderer/bcg_events_points_renderer.h"

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

        if(state->scene.valid(state->picker.entity_id) && state->scene.has<component_selection_vertex_overlay>(state->picker.entity_id)){
            auto &selection = state->scene().get<component_selection_vertex_overlay>(state->picker.entity_id);
            bool show_selection = state->scene.has<event::points_renderer::enqueue>(selection.overlay_id);
            if(ImGui::Checkbox("Show Vertex Selection", &show_selection)){
                if(show_selection){
                    state->scene().emplace<event::points_renderer::enqueue>(selection.overlay_id);
                }else{
                    state->scene().remove<event::points_renderer::enqueue>(selection.overlay_id);
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
            gui_selected_entities(state);
        }
    }

}

}