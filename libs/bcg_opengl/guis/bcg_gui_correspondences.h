//
// Created by alex on 07.12.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_CORRESPONDENCES_H
#define BCG_GRAPHICS_BCG_GUI_CORRESPONDENCES_H

#include "entt/entt.hpp"
#include "geometry/correspondences/bcg_correspondences.h"

namespace bcg{

struct viewer_state;

void gui_correspondence_stats(viewer_state *state, const correspondences &corrs);

void gui_correspondences(viewer_state *state);

}

#endif //BCG_GRAPHICS_BCG_GUI_CORRESPONDENCES_H
