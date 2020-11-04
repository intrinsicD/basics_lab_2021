//
// Created by alex on 28.10.20.
//

#include "bcg_transform_system.h"
#include "bcg_viewer_state.h"

namespace bcg {

transform_system::transform_system(viewer_state *state) : system("transform_system", state) {
    state->dispatcher.sink<event::transform::add>().connect<&transform_system::on_add>(this);
    state->dispatcher.sink<event::transform::translate>().connect<&transform_system::on_translate>(this);
    state->dispatcher.sink<event::transform::scale>().connect<&transform_system::on_scale>(this);
    state->dispatcher.sink<event::transform::rotate>().connect<&transform_system::on_rotate>(this);
}

void transform_system::on_add(const event::transform::add &event){
    if (state->scene.valid(event.id) && !state->scene.has<Transform>(event.id)) {
        state->scene.emplace<Transform>(event.id, Transform::Identity());
    }
}

void transform_system::on_translate(const event::transform::translate &event) {
    if (state->scene.valid(event.id)) {
        auto &model = state->scene.get<Transform>(event.id);
        model = Translation(event.trans) * model;
    }
}

void transform_system::on_scale(const event::transform::scale &event) {
    if (state->scene.valid(event.id)) {
        auto &model = state->scene.get<Transform>(event.id);
        model = model * Scaling(event.scales);
    }
}

void transform_system::on_rotate(const event::transform::rotate &event) {
    if (state->scene.valid(event.id)) {
        auto &model = state->scene.get<Transform>(event.id);
        auto world_translation = Translation(model.translation());

        model = world_translation *
                Rotation(event.angle_axis[0], event.angle_axis.tail<3>()) *
                world_translation.inverse() *
                model;
    }
}
}