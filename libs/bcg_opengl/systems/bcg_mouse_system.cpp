//
// Created by Alex on 02.11.20.
//

#include "bcg_mouse_system.h"
#include "../bcg_viewer_state.h"

namespace bcg{

mouse_system::mouse_system(viewer_state *state) : system("mouse_system", ){

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
    state->mouse.cursor_delta = state->mouse.cursor_position - state->mouse.last_cursor_position;
    state->mouse.is_moving = true;
}

void mouse_system::on_scroll(const event::mouse::scroll &event){
    state->mouse.scroll_value = event.value;
    state->mouse.is_scrolling = true;
}

void mouse_system::on_update(const event::update &event){
    state->mouse.is_dragging =
            state->mouse.is_moving && (state->mouse.left || state->mouse.middle || state->mouse.right);

}

}