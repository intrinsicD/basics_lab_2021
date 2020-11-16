//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_MATERIAL_GRAPH_H
#define BCG_GRAPHICS_BCG_GUI_MATERIAL_GRAPH_H

#include "entt/entt.hpp"
#include "renderers/graph_renderer/bcg_material_graph.h"

namespace bcg{

struct viewer_state;

void gui_material_graph(viewer_state *state, material_graph *material, entt::entity id);

}

#endif //BCG_GRAPHICS_BCG_GUI_MATERIAL_GRAPH_H
