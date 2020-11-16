//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_MATERIAL_VECTORFIELD_H
#define BCG_GRAPHICS_BCG_GUI_MATERIAL_VECTORFIELD_H

#include "entt/entt.hpp"
#include "renderers/vectorfield_renderer/bcg_material_vectorfield.h"

namespace bcg{

struct viewer_state;
struct property_container;

void gui_material_vectorfield(viewer_state *state, material_vectorfield *material, property_container *container,
                              entt::entity id, std::string vectorfield_name);

}

#endif //BCG_GRAPHICS_BCG_GUI_MATERIAL_VECTORFIELD_H
