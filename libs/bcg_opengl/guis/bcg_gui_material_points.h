//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_MATERIAL_POINTS_H
#define BCG_GRAPHICS_BCG_GUI_MATERIAL_POINTS_H

#include "entt/entt.hpp"
#include "renderers/points_renderer/bcg_material_points.h"

namespace bcg{

struct viewer_state;

void gui_material_points(viewer_state *state, material_points *material, entt::entity id);

}

#endif //BCG_GRAPHICS_BCG_GUI_MATERIAL_POINTS_H
