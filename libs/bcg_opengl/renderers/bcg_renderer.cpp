//
// Created by alex on 29.10.20.
//

#include "bcg_renderer.h"
#include "bcg_viewer_state.h"

namespace bcg{

renderer::renderer(viewer_state *state) : system("renderer", state){

}

renderer::renderer(std::string name, viewer_state *state) : system(name, state){
    state->dispatcher.sink<event::begin_frame>().connect<&renderer::on_begin_frame>(this);
    state->dispatcher.sink<event::render>().connect<&renderer::on_render>(this);
    state->dispatcher.sink<event::end_frame>().connect<&renderer::on_end_frame>(this);
}

}