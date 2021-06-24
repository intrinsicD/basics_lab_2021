//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_MESH_H
#define BCG_GRAPHICS_BCG_GUI_MESH_H

#include "entt/entt.hpp"

#include "geometry/mesh/bcg_mesh.h"

namespace bcg{

struct viewer_state;

void gui_mesh(viewer_state *state);

void gui_mesh(viewer_state *state, entt::entity id);

void gui_mesh(viewer_state *state, halfedge_mesh &mesh);


}

#endif //BCG_GRAPHICS_BCG_GUI_MESH_H
