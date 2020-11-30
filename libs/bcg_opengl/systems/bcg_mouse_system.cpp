//
// Created by Alex on 02.11.20.
//

#include "bcg_mouse_system.h"
#include "../bcg_viewer_state.h"

namespace bcg {

mouse_system::mouse_system(viewer_state *state) : system("mouse_system", state) {
    state->dispatcher.sink<event::internal::mouse::button>().connect<&mouse_system::on_button>(this);
    state->dispatcher.sink<event::internal::mouse::motion>().connect<&mouse_system::on_motion>(this);
    state->dispatcher.sink<event::internal::mouse::scroll>().connect<&mouse_system::on_scroll>(this);
    state->dispatcher.sink<event::internal::end_frame>().connect<&mouse_system::on_end_frame>(this);
}

void mouse_system::on_button(const event::internal::mouse::button &event) {
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
    state->mouse.is_dragging =
            state->mouse.is_moving && (state->mouse.left || state->mouse.middle || state->mouse.right);
    state->dispatcher.trigger<event::mouse::button>(event.button, event.action);
}

void mouse_system::on_motion(const event::internal::mouse::motion &event) {
    state->mouse.last_cursor_position = state->mouse.cursor_position;
    state->mouse.cursor_position[0] = event.x * state->window.high_dpi_scaling;
    state->mouse.cursor_position[1] = event.y * state->window.high_dpi_scaling;
    const auto &vp = state->window.framebuffer_viewport;
    state->mouse.window_coordinates[0] = state->mouse.cursor_position[0];
    state->mouse.window_coordinates[1] = vp[3] - state->mouse.cursor_position[1];

    state->mouse.normalized_device_coordinates[0] =
            (state->mouse.window_coordinates[0] - vp[0]) / bcg_scalar_t(vp[2]) * 2.0 - 1.0;
    state->mouse.normalized_device_coordinates[1] =
            (state->mouse.window_coordinates[1] - vp[1]) / bcg_scalar_t(vp[3]) * 2.0 - 1.0;

    VectorS<4> p = (state->cam.projection_matrix() * state->cam.view_matrix()).inverse() *
                   VectorS<4>(state->mouse.normalized_device_coordinates[0],
                              state->mouse.normalized_device_coordinates[1], 0.999885023 * 2.0 - 1.0, 1.0);
    p /= p[3];
    state->mouse.world_space_position = p.head<3>();

    state->mouse.cursor_delta = state->mouse.cursor_position - state->mouse.last_cursor_position;
    state->mouse.is_moving = true;
    state->mouse.is_dragging =
            state->mouse.is_moving && (state->mouse.left || state->mouse.middle || state->mouse.right);
    state->dispatcher.trigger<event::mouse::motion>(state->mouse.cursor_position[0], state->mouse.cursor_position[1]);
}

void mouse_system::on_scroll(const event::internal::mouse::scroll &event) {
    state->mouse.scroll_value = event.value;
    state->mouse.is_scrolling = true;
    state->dispatcher.trigger<event::mouse::scroll>(event.value);
}

void mouse_system::on_end_frame(const event::internal::end_frame &event){
    state->mouse.cursor_delta.setZero();
}

}