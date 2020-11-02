//
// Created by alex on 28.10.20.
//

#include "bcg_camera_system.h"

namespace bcg {

camera_system::camera_system(viewer_state *state) : system("camera_system", state) {
    state->dispatcher.sink<event::startup>().connect<&camera_system::on_startup>(this);
    state->dispatcher.sink<event::update>().connect<&camera_system::on_update>(this);
}

void camera_system::on_startup(const event::startup &event){
    state->cam.init();
}


void camera_system::on_resize(const event::resize &event){
    state->cam.aspect = (bcg_scalar_t) event.width / (bcg_scalar_t) event.height;
}


void camera_system::on_update(const event::update &event) {
    state->cam.update_projection();
}

}