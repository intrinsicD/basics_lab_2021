//
// Created by alex on 14.04.21.
//

#include "bcg_gui_selected_entities.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_entity_info.h"

namespace bcg{

void gui_selected_entities(viewer_state *state){
    if(!state->picker.selected_entities.empty()){
        std::vector<std::string> entitiy_strings;
        for (const auto &item : state->picker.selected_entities){
            auto &info = state->scene.get<entity_info>(item.second);
            std::string entity_name = "entity";
            if(!info.entity_name.empty()){
                entity_name = info.entity_name;
            }
            entity_name += " " + std::to_string(item.first);
            entitiy_strings.push_back(entity_name);
        }

        static int item_current = 1;
        if (ImGui::ListBox(
                "selected_entities", &item_current,
                [](void *data, int idx, const char **out_text) -> bool {
                    auto &entitiy_strings = *(std::vector<std::string> *) data;
                    *out_text = entitiy_strings[idx].c_str();
                    return true;
                },
                &entitiy_strings, (int) entitiy_strings.size(), 4)) {
        }
    }
}

}