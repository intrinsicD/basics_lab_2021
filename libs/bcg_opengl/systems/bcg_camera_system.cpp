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
}

void camera_system::on_startup(const event::internal::startup &event) {
    state->cam.init();
}

void camera_system::on_resize(const event::internal::resize &event) {
    state->cam.aspect = (bcg_scalar_t) event.width / (bcg_scalar_t) event.height;
}

void camera_system::on_mouse_scroll(const event::mouse::scroll &event) {
    if(state->mouse.is_captured_by_gui) return;
    state->cam.fovy_degrees = std::min(std::max<bcg_scalar_t>(state->cam.fovy_degrees + event.value, 1.0), 90.0);
}

void camera_system::on_update(const event::internal::update &event) {
    if (state->mouse.is_dragging && !state->mouse.is_captured_by_gui) {
        if (state->mouse.middle) {
            //translate camera in plane
            state->cam.model_matrix =
                    (world_translation(state->mouse.cursor_delta,
                                       state->window.width,
                                       state->window.height,
                                       state->cam).inverse()) * state->cam.model_matrix;
        }
        if (state->mouse.left) {
            //rotate camera around target point

            state->cam.model_matrix =
                    Translation(state->cam.target_point) *
                    (world_rotation(state->mouse.cursor_position,
                                    state->window.width,
                                    state->window.height,
                                    state->cam)) *
                    Translation(state->cam.target_point).inverse() *
                    state->cam.model_matrix;
        }
    }
    state->cam.update_projection();
}

}