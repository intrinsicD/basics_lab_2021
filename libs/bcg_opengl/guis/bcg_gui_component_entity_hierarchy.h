//
// Created by alex on 19.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_ENTITY_HIERARCHY_H
#define BCG_GRAPHICS_BCG_GUI_ENTITY_HIERARCHY_H

#include "entt/entt.hpp"
#include "components/bcg_component_entity_hierarchy.h"

namespace bcg{

struct viewer_state;

void gui_component_entity_hierarchy(viewer_state *state, entity_hierarchy *hierarchy);

}

#endif //BCG_GRAPHICS_BCG_GUI_ENTITY_HIERARCHY_H
