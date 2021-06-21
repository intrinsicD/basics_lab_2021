//
// Created by alex on 17.06.21.
//

#ifndef BCG_GRAPHICS_BCG_GUI_GUIZMO_H
#define BCG_GRAPHICS_BCG_GUI_GUIZMO_H

#include "math/bcg_linalg.h"
#include "entt/entt.hpp"

namespace bcg{

struct viewer_state;

bool gui_guizmo(viewer_state *state, entt::entity id, Transform &transform);

}
#endif //BCG_GRAPHICS_BCG_GUI_GUIZMO_H
