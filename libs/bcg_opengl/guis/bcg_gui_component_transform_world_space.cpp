//
// Created by alex on 22.06.21.
//

#include "bcg_gui_component_transform_world_space.h"
#include "bcg_gui_edit_transform.h"
#include "components/bcg_component_transform.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg {

void gui_component_transform_world_space(viewer_state *state) {
    gui_component_transform_world_space(state, state->picker.entity_id);
}

void gui_component_transform_world_space(viewer_state *state, entt::entity id) {
    if (!state->scene.valid(id)) return;
    if (!state->scene.has<component_transform>(id)) return;
    Transform &wsm = state->scene.get<component_transform>(id);
    Transform model = state->scene.get_entity_parents_transform(id);
    gui_edit_transform(state, state->cam, model, wsm);
}

}