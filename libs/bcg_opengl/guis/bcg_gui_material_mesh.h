//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_MATERIAL_MESH_H
#define BCG_GRAPHICS_BCG_GUI_MATERIAL_MESH_H

#include "entt/entt.hpp"
#include "renderers/mesh_renderer/bcg_material_mesh.h"

namespace bcg{

struct viewer_state;

void gui_material_mesh(viewer_state *state, material_mesh *material, entt::entity id);

}

#endif //BCG_GRAPHICS_BCG_GUI_MATERIAL_MESH_H
