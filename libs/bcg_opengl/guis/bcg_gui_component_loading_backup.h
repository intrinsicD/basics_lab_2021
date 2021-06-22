//
// Created by alex on 22.06.21.
//

#ifndef BCG_GRAPHICS_BCG_GUI_COMPONENT_LOADING_BACKUP_H
#define BCG_GRAPHICS_BCG_GUI_COMPONENT_LOADING_BACKUP_H

#include "entt/entt.hpp"

namespace bcg{

struct viewer_state;

void gui_component_loading_backup(viewer_state * state);

void gui_component_loading_backup(viewer_state * state, entt::entity id);

}

#endif //BCG_GRAPHICS_BCG_GUI_COMPONENT_LOADING_BACKUP_H
