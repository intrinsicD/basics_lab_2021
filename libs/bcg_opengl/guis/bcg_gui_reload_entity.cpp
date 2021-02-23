//
// Created by alex on 23.02.21.
//

#include "bcg_gui_reload_entity.h"
#include "bcg_viewer_state.h"
#include "bcg_entity_info.h"

namespace bcg{

void gui_reload_entity(viewer_state *state){
    auto id = state->picker.entity_id;
    if(state->scene.valid(id)){
        if(state->scene.has<entity_info>(id)){
            auto &info = state->scene.get<entity_info>(id);
            if(!info.filename.empty()){
                auto filename = info.filename;
                if(ImGui::Button("Reload Entity")){
                    state->dispatcher.trigger<event::internal::destroy>(id);
                    state->scene.destroy(id);
                    state->picker.entity_id = entt::null;
                    std::vector<std::string> filenames = {filename};
                    state->dispatcher.trigger<event::internal::file_drop>(filenames);
                }
            }
        }
    }
}

}
