//
// Created by Alex on 02.11.20.
//

#include "bcg_mouse_system.h"
#include "../bcg_viewer_state.h"

namespace bcg{

mouse_system::mouse_system(viewer_state *state) : system("mouse_system", state){
    state->dispatcher.sink<event::mouse::button>().connect<&mouse_system::on_button>(this);
    state->dispatcher.sink<event::mouse::motion>().connect<&mouse_system::on_motion>(this);
    state->dispatcher.sink<event::mouse::scroll>().connect<&mouse_system::on_scroll>(this);
    state->dispatcher.sink<event::internal::update>().connect<&mouse_system::on_update>(this);
}

void mouse_system::on_button(const event::mouse::button &event){
    state->mouse.buttons[event.button] = event.action;

    state->mouse.left = state->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT];
    if (state->mouse.left) {
        state->mouse.last_left_click = state->mouse.cursor_position;
    }
    state->mouse.middle = state->mouse.buttons[GLFW_MOUSE_BUTTON_MIDDLE];
    if (state->mouse.middle) {
        state->mouse.last_middle_click = state->mouse.cursor_position;
    }
    state->mouse.right = state->mouse.buttons[GLFW_MOUSE_BUTTON_RIGHT];
    if (state->mouse.right) {
        state->mouse.last_right_click = state->mouse.cursor_position;
    }
}

void mouse_system::on_motion(const event::mouse::motion &event){
    state->mouse.last_cursor_position = state->mouse.cursor_position;
    state->mouse.cursor_position[0] = event.x;
    state->mouse.cursor_position[1] = event.y;
    const auto &vp = state->window.framebuffer_viewport;
    state->mouse.normalized_device_coordinates[0] = (event.x - vp[0]) / bcg_scalar_t (vp[2]) * 2.0 - 1.0;
    state->mouse.normalized_device_coordinates[1] = 1.0 - (event.y - vp[1]) / bcg_scalar_t (vp[3]) * 2.0;
    state->mouse.cursor_delta = state->mouse.cursor_position - state->mouse.last_cursor_position;
    state->mouse.is_moving = true;
}

void mouse_system::on_scroll(const event::mouse::scroll &event){
    state->mouse.scroll_value = event.value;
    state->mouse.is_scrolling = true;
}

void mouse_system::on_update(const event::internal::update &event){
    state->mouse.is_dragging =
            state->mouse.is_moving && (state->mouse.left || state->mouse.middle || state->mouse.right);

}

}