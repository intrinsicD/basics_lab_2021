//
// Created by alex on 17.06.21.
//

#include "bcg_object_space_system.h"
#include "viewer/bcg_viewer_state.h"
#include "components/bcg_component_object_space_view.h"
#include "aligned_box/bcg_aligned_box.h"
#include "guis/bcg_gui_guizmo.h"

namespace bcg{

object_space_system::object_space_system(viewer_state *state) : system("object_space_system", state){
    state->dispatcher.sink<event::object_space::add_component_object_space_transform>().connect<&object_space_system::on_add_object_space_transform>(this);
    state->dispatcher.sink<event::object_space::set_component_object_space_transform>().connect<&object_space_system::on_set_object_space_transform>(this);
    state->dispatcher.sink<event::object_space::translate_aabb_center_to_origin>().connect<&object_space_system::on_object_space_translate_aabb_center_to_origin>(this);
    state->dispatcher.sink<event::object_space::uniform_scale_to_unit_cube>().connect<&object_space_system::on_object_space_uniform_scale_to_unit_cube>(this);
}

bool check_valid(viewer_state *state, entt::entity id){
    return state->scene.valid(id) && state->scene.all_of<object_space_view>(id);
}

void object_space_system::on_add_object_space_transform(const event::object_space::add_component_object_space_transform &event){
    if(!state->scene.valid(event.id)) return;
    state->scene.emplace_or_replace<object_space_view>(event.id, object_space_view());
}

void object_space_system::on_set_object_space_transform(const event::object_space::set_component_object_space_transform &event){
    if(!state->scene.valid(event.id)) return;
    state->scene.emplace_or_replace<object_space_view>(event.id, event.transform);
}


void object_space_system::on_object_space_translate_aabb_center_to_origin(const event::object_space::translate_aabb_center_to_origin &event){
    if(!check_valid(state, event.id)) return;
    if(!state->scene.all_of<aligned_box3>(event.id)) return;
    auto &aabb = state->scene.get<aligned_box3>(event.id);
    object_space_view &transform = state->scene.get<object_space_view>(event.id);
    transform = object_space_view(Transform(Translation(-aabb.center())));
    aabb.min = aabb.min - aabb.center();
    aabb.max = aabb.max - aabb.center();
}

void object_space_system::on_object_space_uniform_scale_to_unit_cube(const event::object_space::uniform_scale_to_unit_cube &event){
    if(!check_valid(state, event.id)) return;
    if(!state->scene.all_of<aligned_box3>(event.id)) return;
    auto &aabb = state->scene.get<aligned_box3>(event.id);
    object_space_view &transform = state->scene.get<object_space_view>(event.id);
    bcg_scalar_t u_scale = 1.0 / aabb.halfextent().maxCoeff();
    transform = object_space_view(Transform(Scaling(u_scale, u_scale, u_scale)));
    aabb.min = aabb.min * u_scale;
    aabb.max = aabb.max * u_scale;
}


}