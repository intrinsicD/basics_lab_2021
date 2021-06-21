//
// Created by alex on 23.02.21.
//

#include "bcg_gui_reload_entity.h"
#include "viewer/bcg_viewer_state.h"
#include "components/bcg_component_entity_info.h"

namespace bcg{

void gui_reload_entity(viewer_state *state){
    if(state->scene.valid(state->picker.entity_id)){
        if(state->scene.all_of<entity_info>(state->picker.entity_id)){
            auto &info = state->scene.get<entity_info>(state->picker.entity_id);
            if(!info.filename.empty()){
                auto filename = info.filename;
                if(ImGui::Button("Reload Entity")){
                    state->dispatcher.trigger<event::internal::destroy>(state->picker.entity_id);
                    state->scene.destroy(state->picker.entity_id);
                    state->picker.entity_id = entt::null;
                    std::vector<std::string> filenames = {filename};
                    state->dispatcher.trigger<event::internal::file_drop>(filenames);
                }
            }
        }
    }
}

}
