//
// Created by alex on 22.04.21.
//

#ifndef BCG_GRAPHICS_BCG_GUI_ROTATION_AVERAGING_H
#define BCG_GRAPHICS_BCG_GUI_ROTATION_AVERAGING_H

#include "math/matrix/bcg_matrix.h"

namespace bcg{

struct viewer_state;

bool gui_rotation_averaging(viewer_state *state, MatrixS<3, 3> &result);

}

#endif //BCG_GRAPHICS_BCG_GUI_ROTATION_AVERAGING_H
