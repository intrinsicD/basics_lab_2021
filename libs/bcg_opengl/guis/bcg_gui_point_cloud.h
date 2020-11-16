//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_POINT_CLOUD_H
#define BCG_GRAPHICS_BCG_GUI_POINT_CLOUD_H

#include "geometry/point_cloud/bcg_point_cloud.h"

namespace bcg{

struct viewer_state;

void gui_point_cloud(viewer_state *state, point_cloud *pc);

}

#endif //BCG_GRAPHICS_BCG_GUI_POINT_CLOUD_H
