//
// Created by alex on 22.03.21.
//

#ifndef BCG_GRAPHICS_BCG_GUI_EXPORTING_H
#define BCG_GRAPHICS_BCG_GUI_EXPORTING_H

namespace bcg{

struct viewer_state;

void gui_exporting(viewer_state *state);

void gui_exporting_mesh(viewer_state *state);

void gui_exporting_point_cloud(viewer_state *state);

}

#endif //BCG_GRAPHICS_BCG_GUI_EXPORTING_H
