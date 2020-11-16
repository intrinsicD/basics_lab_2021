//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_VECTORFIELDS_H
#define BCG_GRAPHICS_BCG_GUI_VECTORFIELDS_H

#include "entt/entt.hpp"
#include "renderers/bcg_vectorfields.h"

namespace bcg{

struct viewer_state;

void gui_vectorfields(viewer_state *state, vectorfields *vectorfields, entt::entity id);

}

#endif //BCG_GRAPHICS_BCG_GUI_VECTORFIELDS_H
