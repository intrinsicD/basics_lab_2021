//
// Created by alex on 28.10.20.
//

#include "bcg_transform_system.h"
#include "viewer/bcg_viewer_state.h"

namespace bcg {

transform_system::transform_system(viewer_state *state) : system("transform_system", state) {
    state->dispatcher.sink<event::transform::add>().connect<&transform_system::on_add>(this);
    state->dispatcher.sink<event::transform::translate>().connect<&transform_system::on_translate>(this);
    state->dispatcher.sink<event::transform::scale>().connect<&transform_system::on_scale>(this);
    state->dispatcher.sink<event::transform::rotate>().connect<&transform_system::on_rotate>(this);
    state->dispatcher.sink<event::internal::update>().connect<&transform_system::on_update>(this);
}

void transform_system::on_add(const event::transform::add &event){
    if (state->scene.valid(event.id) && !state->scene.all_of<Transform>(event.id)) {
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

void transform_system::on_update(const event::internal::update &){
    if (state->mouse.is_dragging && !state->mouse.is_captured_by_gui && state->keyboard.ctrl_pressed) {
        if(!state->picker.valid || !state->scene.all_of<Transform>(state->picker.entity_id)) return;
        auto &model = state->scene.get<Transform>(state->picker.entity_id);
        if (state->mouse.middle) {
            //translate camera in plane

            model = world_translation(state->mouse.cursor_delta,
                                       state->window.width,
                                       state->window.height,
                                       state->cam).inverse() * model;
        }
        if (state->mouse.left) {
            //rotate camera around target point

            model = Translation(model.translation()) *
                    world_rotation(state->mouse.cursor_position,
                                    state->window.width,
                                    state->window.height,
                                    state->cam) *
                    Translation(-model.translation()) *
                    model;
        }
    }
}

}