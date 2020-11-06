//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_VIEWER_H
#define BCG_GRAPHICS_BCG_VIEWER_H

#include "math/bcg_linalg.h"
#include "bcg_viewer_state.h"

namespace bcg {

struct viewer {
    viewer_state state;

    void run(const VectorI<2> &size = {800, 600}, const std::string &title = "default", int widgets_width = 320);
};

}

#endif //BCG_GRAPHICS_BCG_VIEWER_H
