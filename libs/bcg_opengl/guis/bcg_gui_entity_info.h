//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_ENTITY_INFO_H
#define BCG_GRAPHICS_BCG_GUI_ENTITY_INFO_H

#include "entt/entt.hpp"
#include "components/bcg_component_entity_info.h"

namespace bcg{

struct viewer_state;

void gui_entity_info(viewer_state *state, entity_info *info, entt::entity id);

}

#endif //BCG_GRAPHICS_BCG_GUI_ENTITY_INFO_H
