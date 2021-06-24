//
// Created by alex on 24.06.21.
//

#ifndef BCG_GRAPHICS_BCG_GUI_ENTITY_COMPONENTS_H
#define BCG_GRAPHICS_BCG_GUI_ENTITY_COMPONENTS_H

#include "entt/entt.hpp"

namespace bcg{

struct viewer_state;

void gui_entity_components(viewer_state *state);

void gui_entity_components(viewer_state *state, entt::entity id);

}

#endif //BCG_GRAPHICS_BCG_GUI_ENTITY_COMPONENTS_H
