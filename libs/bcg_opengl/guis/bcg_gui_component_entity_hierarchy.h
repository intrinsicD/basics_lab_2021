//
// Created by alex on 19.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_ENTITY_HIERARCHY_H
#define BCG_GRAPHICS_BCG_GUI_ENTITY_HIERARCHY_H

#include "entt/entt.hpp"

namespace bcg{

struct viewer_state;

void gui_component_entity_hierarchy(viewer_state *state);

void gui_component_entity_hierarchy(viewer_state *state, entt::entity id);

}

#endif //BCG_GRAPHICS_BCG_GUI_ENTITY_HIERARCHY_H
