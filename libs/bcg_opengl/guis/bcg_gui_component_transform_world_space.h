//
// Created by alex on 22.06.21.
//

#ifndef BCG_GRAPHICS_BCG_GUI_COMPONENT_TRANSFORM_WORLD_SPACE_H
#define BCG_GRAPHICS_BCG_GUI_COMPONENT_TRANSFORM_WORLD_SPACE_H

#include "entt/entt.hpp"
#include "math/bcg_linalg.h"

namespace bcg{

struct viewer_state;

void gui_component_transform_world_space(viewer_state *state);

void gui_component_transform_world_space(viewer_state *state, entt::entity id);

}


#endif //BCG_GRAPHICS_BCG_GUI_COMPONENT_TRANSFORM_WORLD_SPACE_H
