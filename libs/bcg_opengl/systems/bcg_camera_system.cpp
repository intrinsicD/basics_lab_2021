//
// Created by alex on 28.10.20.
//

#include "bcg_camera_system.h"
#include "bcg_viewer_state.h"

namespace bcg {

camera_system::camera_system(viewer_state *state) : system("camera_system", state) {
    state->dispatcher.sink<event::internal::startup>().connect<&camera_system::on_startup>(this);
    state->dispatcher.sink<event::internal::resize>().connect<&camera_system::on_resize>(this);
    state->dispatcher.sink<event::mouse::scroll>().connect<&camera_system::on_mouse_scroll>(this);
    state->dispatcher.sink<event::internal::update>().connect<&camera_system::on_update>(this);
    state->dispatcher.sink<event::internal::end_frame>().connect<&camera_system::on_end_frame>(this);
}

void camera_system::on_startup(const event::internal::startup &) {
    state->cam.init(state->window.width, state->window.height);
    state->dispatcher.trigger<event::internal::resize>(state->window.width, state->window.height);
}

void camera_system::on_resize(const event::internal::resize &event) {
    state->cam.aspect = (bcg_scalar_t) event.width / (bcg_scalar_t) event.height;
    glViewport(state->window.framebuffer_viewport[0],
               state->window.framebuffer_viewport[1],
               state->window.framebuffer_viewport[2],
               state->window.framebuffer_viewport[3]);
}

void camera_system::on_mouse_scroll(const event::mouse::scroll &event) {
    if (state->mouse.is_captured_by_gui) return;
    state->cam.fovy_degrees = std::min<bcg_scalar_t>(std::max<bcg_scalar_t>(state->cam.fovy_degrees - event.value, 1.0),
                                                     90.0);
}

void camera_system::on_update(const event::internal::update &) {
    if (state->mouse.is_dragging && !state->mouse.is_captured_by_gui) {
        if (state->mouse.middle) {
            //translate camera in plane
            state->cam.model_matrix =
                    (world_translation(state->mouse.cursor_delta,
                                       state->window.width,
                                       state->window.height,
                                       state->cam)) * state->cam.model_matrix;
        }
        if (state->mouse.left) {
            //rotate camera around target point

            state->cam.model_matrix =
                    Translation(state->cam.target_point) *
                    (world_rotation(state->mouse.cursor_position,
                                    state->window.width,
                                    state->window.height,
                                    state->cam).inverse()) *
                    Translation(state->cam.target_point) *
                    state->cam.model_matrix;
        }
    }
    state->cam.update_projection();
}

void camera_system::on_end_frame(const event::internal::end_frame &) {
    state->cam.last_point_2d = state->mouse.cursor_position;
    state->cam.last_point_ok = map_to_sphere(state->cam.last_point_2d,
                                             state->window.width,
                                             state->window.height,
                                             state->cam.last_point_3d);

}

}