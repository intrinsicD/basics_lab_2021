//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_VIEWER_H
#define BCG_GRAPHICS_BCG_VIEWER_H

#include "bcg_viewer_state.h"

namespace bcg {

struct viewer {
    viewer_state state;

    void run(int width = 800, int height = 600, const std::string &title = "default", int widgets_width = 500);
};

}

#endif //BCG_GRAPHICS_BCG_VIEWER_H
