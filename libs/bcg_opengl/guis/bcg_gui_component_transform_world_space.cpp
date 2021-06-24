//
// Created by alex on 22.06.21.
//

#include "bcg_gui_component_transform_world_space.h"
#include "bcg_gui_edit_transform.h"
#include "components/bcg_component_transform_world_space.h"
#include "components/bcg_component_transform_object_space.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg{

void gui_component_transform_world_space(viewer_state *state){
    gui_component_transform_world_space(state, state->picker.entity_id);
}

void gui_component_transform_world_space(viewer_state *state, entt::entity id){
    if(!state->scene.valid(id)) return;
    if(!state->scene.has<world_space_transform>(id)) return;
    Transform model = state->scene.get_entity_world_transform(id);
    if(state->scene.has<object_space_transform>(id)){
        auto &osm = state->scene.get<object_space_transform>(id);
        model = model * osm;
    }
    Transform &wsm = state->scene.get<world_space_transform>(id);
    gui_edit_transform(state, state->cam, state->scene.ws_model.inverse() * model * wsm.inverse(), wsm);
}

}