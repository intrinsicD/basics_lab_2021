//
// Created by alex on 16.11.20.
//

#include "bcg_gui_ogl_shape.h"
#include "viewer/bcg_viewer_state.h"
#include "bcg_gui_ogl_buffer.h"

namespace bcg {

void gui_ogl_shape(viewer_state *state) {
    gui_ogl_shape(state, state->picker.entity_id);
}

void gui_ogl_shape(viewer_state *state, entt::entity id) {
    if (!state->scene.valid(id)) return;
    if (!state->scene.has<ogl_shape>(id)) return;
    gui_ogl_shape(state, state->scene.get<ogl_shape>(id));
}

void gui_ogl_shape(viewer_state *state, ogl_shape &shape) {
    for (auto &item : shape.vertex_buffers) {
        auto &buffer = item.second;
        gui_ogl_buffer(state, buffer);
    }
    if (shape.edge_buffer) {
        gui_ogl_buffer(state, shape.edge_buffer);
    }
    if (shape.triangle_buffer) {
        gui_ogl_buffer(state, shape.triangle_buffer);
    }
    if (shape.adjacency_buffer) {
        gui_ogl_buffer(state, shape.adjacency_buffer);
    }
}

}