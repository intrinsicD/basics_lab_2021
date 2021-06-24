//
// Created by alex on 22.06.21.
//

#ifndef BCG_GRAPHICS_BCG_GUI_EDIT_TRANSFORM_H
#define BCG_GRAPHICS_BCG_GUI_EDIT_TRANSFORM_H

#include "math/bcg_linalg.h"
#include "viewer/bcg_camera.h"

namespace bcg{

struct viewer_state;

void gui_edit_transform(viewer_state *state, camera &cam, const Transform &accumulated_model, Transform &transformation);
void gui_edit_transform(viewer_state *state, Transform &transformation);

}

#endif //BCG_GRAPHICS_BCG_GUI_EDIT_TRANSFORM_H
