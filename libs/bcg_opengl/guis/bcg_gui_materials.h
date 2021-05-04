//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_MATERIALS_H
#define BCG_GRAPHICS_BCG_GUI_MATERIALS_H

#include "entt/entt.hpp"

namespace bcg{

struct viewer_state;

void gui_materials(viewer_state *state, entt::entity id, bool show_points, bool show_hq_points, bool show_edges, bool show_meshes, bool show_vectors, bool show_curves);

}

#endif //BCG_GRAPHICS_BCG_GUI_MATERIALS_H
