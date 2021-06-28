//
// Created by Alex on 02.11.20.
//

#include "bcg_keyboard_system.h"
#include "viewer/bcg_viewer_state.h"
#include "components/bcg_component_entity_info.h"

namespace bcg {

keyboard_system::keyboard_system(viewer_state *state) : system("keyboard_system", state) {
    state->dispatcher.sink<event::internal::update>().connect<&keyboard_system::on_update>(this);
    state->dispatcher.sink<event::internal::keyboard>().connect<&keyboard_system::on_keyboard>(this);
}

void keyboard_system::on_keyboard(const event::internal::keyboard &event) {
    /* Use this function to control single action per frame when triggered by glfw
     * like: enable, disable stuff
     * use on_update if you want to update continuously
     * */
    state->keyboard.keys[event.button] = event.action;

    state->keyboard.alt_pressed =
            state->keyboard.keys[GLFW_KEY_LEFT_ALT] || state->keyboard.keys[GLFW_KEY_RIGHT_ALT];
    state->keyboard.shift_pressed =
            state->keyboard.keys[GLFW_KEY_LEFT_SHIFT] || state->keyboard.keys[GLFW_KEY_RIGHT_SHIFT];
    state->keyboard.ctrl_pressed =
            state->keyboard.keys[GLFW_KEY_LEFT_CONTROL] || state->keyboard.keys[GLFW_KEY_RIGHT_CONTROL];
    state->keyboard.command_pressed =
            state->keyboard.keys[GLFW_KEY_LEFT_SUPER] || state->keyboard.keys[GLFW_KEY_RIGHT_SUPER];
    state->keyboard.no_modifier = !(state->keyboard.alt_pressed ||
                                    state->keyboard.shift_pressed ||
                                    state->keyboard.ctrl_pressed ||
                                    state->keyboard.command_pressed);
    if (state->gui.captured_keyboard) return;
    if (state->keyboard.keys[GLFW_KEY_A]) {
        if(state->keyboard.ctrl_pressed){
            auto view = state->scene().view<entity_info>();
            state->picker.selected_entities.clear();
            std::string message;
            for(const auto &id: view){
                state->picker.selected_entities[(size_t)id] = id;
                message += " " + std::to_string(size_t(id));
            }
            std::cout << "selected entities " + message << "\n";
        }
    }
    if (state->keyboard.keys[GLFW_KEY_B]) {}
    if (state->keyboard.keys[GLFW_KEY_C]) {}
    if (state->keyboard.keys[GLFW_KEY_D]) {}
    if (state->keyboard.keys[GLFW_KEY_E]) {}
    if (state->keyboard.keys[GLFW_KEY_F]) {}
    if (state->keyboard.keys[GLFW_KEY_G]) {
        state->gui.hide_all = !state->gui.hide_all;
        state->gui.captured_keyboard = false;
        state->gui.captured_mouse = false;
        state->gui.widgets_active = false;
    }
    if (state->keyboard.keys[GLFW_KEY_H]) {}
    if (state->keyboard.keys[GLFW_KEY_I]) {}
    if (state->keyboard.keys[GLFW_KEY_J]) {}
    if (state->keyboard.keys[GLFW_KEY_K]) {}
    if (state->keyboard.keys[GLFW_KEY_L]) {}
    if (state->keyboard.keys[GLFW_KEY_M]) {}
    if (state->keyboard.keys[GLFW_KEY_N]) {}
    if (state->keyboard.keys[GLFW_KEY_O]) {}
    if (state->keyboard.keys[GLFW_KEY_P]) {}
    if (state->keyboard.keys[GLFW_KEY_Q]) {
        if(state->keyboard.ctrl_pressed){
            glfwSetWindowShouldClose(state->window.win, true);
        }
    }
    if (state->keyboard.keys[GLFW_KEY_R]) {
        if(state->keyboard.ctrl_pressed){
            for(const auto &item : state->picker.selected_entities){
                state->dispatcher.trigger<event::transform::world_space::set_identity>(item.second);
            }
            if(state->scene.valid(state->picker.entity_id)){
                state->dispatcher.trigger<event::transform::world_space::set_identity>(state->picker.entity_id);
            }
        }else{
            state->cam.init(state->window.width, state->window.height);
        }
    }
    if (state->keyboard.keys[GLFW_KEY_S]) {}
    if (state->keyboard.keys[GLFW_KEY_T]) {
        state->dispatcher.trigger<event::mesh::make_parameterized_plane>();
    }
    if (state->keyboard.keys[GLFW_KEY_U]) {}
    if (state->keyboard.keys[GLFW_KEY_V]) {}
    if (state->keyboard.keys[GLFW_KEY_W]) {}
    if (state->keyboard.keys[GLFW_KEY_X]) {}
    if (state->keyboard.keys[GLFW_KEY_Y]) {}
    if (state->keyboard.keys[GLFW_KEY_Z]) {}
    if (state->keyboard.keys[GLFW_KEY_0]) {}
    if (state->keyboard.keys[GLFW_KEY_1]) {}
    if (state->keyboard.keys[GLFW_KEY_2]) {}
    if (state->keyboard.keys[GLFW_KEY_3]) {}
    if (state->keyboard.keys[GLFW_KEY_4]) {}
    if (state->keyboard.keys[GLFW_KEY_5]) {}
    if (state->keyboard.keys[GLFW_KEY_6]) {}
    if (state->keyboard.keys[GLFW_KEY_7]) {}
    if (state->keyboard.keys[GLFW_KEY_8]) {}
    if (state->keyboard.keys[GLFW_KEY_9]) {}
    if (state->keyboard.keys[GLFW_KEY_RIGHT_CONTROL]) {}
    if (state->keyboard.keys[GLFW_KEY_LEFT_CONTROL]) {}
    if (state->keyboard.keys[GLFW_KEY_RIGHT_ALT]) {}
    if (state->keyboard.keys[GLFW_KEY_LEFT_ALT]) {}
    if (state->keyboard.keys[GLFW_KEY_RIGHT_SHIFT]) {}
    if (state->keyboard.keys[GLFW_KEY_LEFT_SHIFT]) {}
    if (state->keyboard.keys[GLFW_KEY_RIGHT_SUPER]) {}
    if (state->keyboard.keys[GLFW_KEY_LEFT_SUPER]) {}
    if (state->keyboard.keys[GLFW_KEY_ENTER]) {}
    if (state->keyboard.keys[GLFW_KEY_SPACE]) {}
    if (state->keyboard.keys[GLFW_KEY_BACKSPACE]) {}
    if (state->keyboard.keys[GLFW_KEY_DELETE]) {
        if (state->picker.valid && state->scene.valid(state->picker.entity_id)) {
            state->dispatcher.trigger<event::internal::destroy>(state->picker.entity_id);
            state->scene().destroy(state->picker.entity_id);
            state->picker.entity_id = entt::null;
        }
    }
    if (state->keyboard.keys[GLFW_KEY_F1]) {}
    if (state->keyboard.keys[GLFW_KEY_F2]) {}
    if (state->keyboard.keys[GLFW_KEY_F3]) {}
    if (state->keyboard.keys[GLFW_KEY_F4]) {}
    if (state->keyboard.keys[GLFW_KEY_F5]) {}
    if (state->keyboard.keys[GLFW_KEY_F6]) {}
    if (state->keyboard.keys[GLFW_KEY_F7]) {}
    if (state->keyboard.keys[GLFW_KEY_F8]) {}
    if (state->keyboard.keys[GLFW_KEY_F9]) {}
}

void keyboard_system::on_update(const event::internal::update &) {
    /* Use this function to control continuous action per frame when updated by viewer
     * like: moving around with w,a,s,d.
     * use on_keyboard if you want to enable, disable stuff
    * */
    if (state->keyboard.keys[GLFW_KEY_A]) {
        if(state->keyboard.no_modifier){
            state->cam.model_matrix.translation() -= state->cam.right_vec() * state->time.time_delta * state->cam.mov_speed;
        }
    }
    if (state->keyboard.keys[GLFW_KEY_B]) {}
    if (state->keyboard.keys[GLFW_KEY_C]) {}
    if (state->keyboard.keys[GLFW_KEY_D]) {
        if(state->keyboard.no_modifier) {
            state->cam.model_matrix.translation() += state->cam.right_vec() * state->time.time_delta * state->cam.mov_speed;
        }
    }
    if (state->keyboard.keys[GLFW_KEY_E]) {}
    if (state->keyboard.keys[GLFW_KEY_F]) {}
    if (state->keyboard.keys[GLFW_KEY_G]) {}
    if (state->keyboard.keys[GLFW_KEY_H]) {}
    if (state->keyboard.keys[GLFW_KEY_I]) {}
    if (state->keyboard.keys[GLFW_KEY_J]) {}
    if (state->keyboard.keys[GLFW_KEY_K]) {}
    if (state->keyboard.keys[GLFW_KEY_L]) {}
    if (state->keyboard.keys[GLFW_KEY_M]) {}
    if (state->keyboard.keys[GLFW_KEY_N]) {}
    if (state->keyboard.keys[GLFW_KEY_O]) {}
    if (state->keyboard.keys[GLFW_KEY_P]) {}
    if (state->keyboard.keys[GLFW_KEY_Q]) {}
    if (state->keyboard.keys[GLFW_KEY_R]) {}
    if (state->keyboard.keys[GLFW_KEY_S]) {
        if(state->keyboard.no_modifier) {
            state->cam.model_matrix.translation() += state->cam.direction_vec() * state->time.time_delta * state->cam.mov_speed;
        }
    }
    if (state->keyboard.keys[GLFW_KEY_T]) {}
    if (state->keyboard.keys[GLFW_KEY_U]) {}
    if (state->keyboard.keys[GLFW_KEY_V]) {}
    if (state->keyboard.keys[GLFW_KEY_W]) {
        if(state->keyboard.no_modifier) {
            state->cam.model_matrix.translation() -= state->cam.direction_vec() * state->time.time_delta * state->cam.mov_speed;
        }
    }
    if (state->keyboard.keys[GLFW_KEY_X]) {}
    if (state->keyboard.keys[GLFW_KEY_Y]) {}
    if (state->keyboard.keys[GLFW_KEY_Z]) {}
    if (state->keyboard.keys[GLFW_KEY_0]) {}
    if (state->keyboard.keys[GLFW_KEY_1]) {}
    if (state->keyboard.keys[GLFW_KEY_2]) {}
    if (state->keyboard.keys[GLFW_KEY_3]) {}
    if (state->keyboard.keys[GLFW_KEY_4]) {}
    if (state->keyboard.keys[GLFW_KEY_5]) {}
    if (state->keyboard.keys[GLFW_KEY_6]) {}
    if (state->keyboard.keys[GLFW_KEY_7]) {}
    if (state->keyboard.keys[GLFW_KEY_8]) {}
    if (state->keyboard.keys[GLFW_KEY_9]) {}
    if (state->keyboard.keys[GLFW_KEY_RIGHT_CONTROL]) {}
    if (state->keyboard.keys[GLFW_KEY_LEFT_CONTROL]) {}
    if (state->keyboard.keys[GLFW_KEY_RIGHT_ALT]) {}
    if (state->keyboard.keys[GLFW_KEY_LEFT_ALT]) {}
    if (state->keyboard.keys[GLFW_KEY_RIGHT_SHIFT]) {}
    if (state->keyboard.keys[GLFW_KEY_LEFT_SHIFT]) {}
    if (state->keyboard.keys[GLFW_KEY_RIGHT_SUPER]) {}
    if (state->keyboard.keys[GLFW_KEY_LEFT_SUPER]) {}
    if (state->keyboard.keys[GLFW_KEY_ENTER]) {}
    if (state->keyboard.keys[GLFW_KEY_SPACE]) {}
    if (state->keyboard.keys[GLFW_KEY_BACKSPACE]) {}
    if (state->keyboard.keys[GLFW_KEY_DELETE]) {}
    if (state->keyboard.keys[GLFW_KEY_F1]) {}
    if (state->keyboard.keys[GLFW_KEY_F2]) {}
    if (state->keyboard.keys[GLFW_KEY_F3]) {}
    if (state->keyboard.keys[GLFW_KEY_F4]) {}
    if (state->keyboard.keys[GLFW_KEY_F5]) {}
    if (state->keyboard.keys[GLFW_KEY_F6]) {}
    if (state->keyboard.keys[GLFW_KEY_F7]) {}
    if (state->keyboard.keys[GLFW_KEY_F8]) {}
    if (state->keyboard.keys[GLFW_KEY_F9]) {}
}

}