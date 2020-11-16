//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_GRAPH_H
#define BCG_GRAPHICS_BCG_GUI_GRAPH_H

#include "geometry/graph/bcg_graph.h"

namespace bcg{

struct viewer_state;

void gui_graph(viewer_state *state, halfedge_graph *graph);

}

#endif //BCG_GRAPHICS_BCG_GUI_GRAPH_H
