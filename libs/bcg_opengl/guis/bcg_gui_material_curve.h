//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_MATERIAL_CURVE_H
#define BCG_GRAPHICS_BCG_GUI_MATERIAL_CURVE_H

#include "entt/entt.hpp"
#include "renderers/curve_renderer/bcg_material_curve.h"

namespace bcg{

struct viewer_state;

void gui_material_curve(viewer_state *state, material_curve *material, entt::entity id);

}

#endif //BCG_GRAPHICS_BCG_GUI_MATERIAL_CURVE_H
