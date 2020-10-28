//
// Created by alex on 28.10.20.
//

#include "bcg_camera_system.h"

namespace bcg{

camera_system::camera_system(viewer_state *state) : system("camera_system", state){
state->dispatcher.sink<event::update>().connect<&camera_system::on_update>(this);
}

void camera_system::on_update(const event::update &event){
    state->cam.update_projection();
}

}