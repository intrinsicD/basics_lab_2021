//
// Created by alex on 22.06.21.
//

#include "bcg_gui_component_transform_world_space.h"
#include "bcg_gui_edit_transform.h"
#include "components/bcg_component_transform_world_space.h"
#include "components/bcg_component_transform_object_space.h"
#include "components/bcg_component_entity_hierarchy.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg{

void gui_component_transform_world_space(viewer_state *state){
    gui_component_transform_world_space(state, state->picker.entity_id);
}

void gui_component_transform_world_space(viewer_state *state, entt::entity id){
    if(!state->scene.valid(id)) return;
    if(!state->scene.has<world_space_transform>(id)) return;
    Transform &wsm = state->scene.get<world_space_transform>(id);
    Transform model = wsm.inverse();
    if(state->scene.has<entity_hierarchy>(id)){
        auto &hierarchy = state->scene.get<entity_hierarchy>(id);
        model = hierarchy.accumulated_model * model;
    }
    model = state->scene.ws_model * model;
    Transform translation = Transform(Translation(wsm.translation()));
    gui_edit_transform(state, state->cam, translation.inverse(),wsm);
}

}