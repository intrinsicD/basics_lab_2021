//
// Created by alex on 16.11.20.
//

#ifndef BCG_GRAPHICS_BCG_GUI_OGL_BUFFER_H
#define BCG_GRAPHICS_BCG_GUI_OGL_BUFFER_H

#include "viewer/bcg_opengl.h"

namespace bcg{

struct viewer_state;

void gui_ogl_buffer(viewer_state* state, const ogl_buffer_object &buffer);

}

#endif //BCG_GRAPHICS_BCG_GUI_OGL_BUFFER_H
